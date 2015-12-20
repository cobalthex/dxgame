#include "pch.hpp"
#include "DeviceResources.hpp"
#include "DirectXHelper.hpp"

using namespace DirectX;
using namespace Microsoft::WRL;
using namespace Windows::Foundation;
using namespace Windows::Graphics::Display;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml::Controls;
using namespace Platform;

// Constants used to calculate screen rotations.
namespace ScreenRotation
{
	// 0-degree Z-rotation
	static const XMFLOAT4X4 Rotation0(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
		);

	// 90-degree Z-rotation
	static const XMFLOAT4X4 Rotation90(
		0.0f, 1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
		);

	// 180-degree Z-rotation
	static const XMFLOAT4X4 Rotation180(
		-1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
		);

	// 270-degree Z-rotation
	static const XMFLOAT4X4 Rotation270(
		0.0f, -1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
		);
};

// Constructor for DeviceResources.
DX::DeviceResources::DeviceResources() :
	currentFrame(0),
	screenViewport(),
	rtvDescriptorSize(0),
	fenceEvent(0),
	d3dRenderTargetSize(),
	outputSize(),
	logicalSize(),
	nativeOrientation(DisplayOrientations::None),
	currentOrientation(DisplayOrientations::None),
	dpi(-1.0f),
	deviceRemoved(false)
{
	ZeroMemory(fenceValues, sizeof(fenceValues));
	CreateDeviceIndependentResources();
	CreateDeviceResources();
}

// Configures resources that don't depend on the Direct3D device.
void DX::DeviceResources::CreateDeviceIndependentResources()
{
}

// Configures the Direct3D device, and stores handles to it and the device context.
void DX::DeviceResources::CreateDeviceResources()
{
#if defined(_DEBUG)
	// If the project is in a debug build, enable debugging via SDK Layers.
	{
		ComPtr<ID3D12Debug> debugController;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
		{
			debugController->EnableDebugLayer();
		}
	}
#endif

	DX::ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory)));

	// Create the Direct3D 12 API device object
	HRESULT hr = D3D12CreateDevice(
		nullptr,						// Specify nullptr to use the default adapter.
		D3D_FEATURE_LEVEL_12_0,			// Minimum feature level this app can support.
		IID_PPV_ARGS(&d3dDevice)		// Returns the Direct3D device created.
		);

	if (FAILED(hr))
	{
		// If the initialization fails, fall back to the WARP device.
		// For more information on WARP, see: 
		// http://go.microsoft.com/fwlink/?LinkId=286690

		ComPtr<IDXGIAdapter> warpAdapter;
		dxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter)); 

		DX::ThrowIfFailed(
			D3D12CreateDevice(
				warpAdapter.Get(),
				D3D_FEATURE_LEVEL_11_0,
				IID_PPV_ARGS(&d3dDevice)
				)
			);
	}

	// Create the command queue.
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	DX::ThrowIfFailed(d3dDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&commandQueue)));

	for (UINT n = 0; n < c_frameCount; n++)
	{
		DX::ThrowIfFailed(
			d3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocators[n]))
			);
	}

	// Create synchronization objects.
	DX::ThrowIfFailed(d3dDevice->CreateFence(fenceValues[currentFrame], D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)));
	fenceValues[currentFrame]++;

	fenceEvent = CreateEventEx(nullptr, FALSE, FALSE, EVENT_ALL_ACCESS);
}

// These resources need to be recreated every time the window size is changed.
void DX::DeviceResources::CreateWindowSizeDependentResources()
{
	// Wait until all previous GPU work is complete.
	WaitForGpu();

	// Clear the previous window size specific content.
	for (UINT n = 0; n < c_frameCount; n++)
	{
		renderTargets[n] = nullptr;
	}
	rtvHeap = nullptr;

	// Calculate the necessary render target size in pixels.
	outputSize.Width = DX::ConvertDipsToPixels(logicalSize.Width, dpi);
	outputSize.Height = DX::ConvertDipsToPixels(logicalSize.Height, dpi);

	// Prevent zero size DirectX content from being created.
	outputSize.Width = max(outputSize.Width, 1);
	outputSize.Height = max(outputSize.Height, 1);

	// The width and height of the swap chain must be based on the window's
	// natively-oriented width and height. If the window is not in the native
	// orientation, the dimensions must be reversed.
	DXGI_MODE_ROTATION displayRotation = ComputeDisplayRotation();

	bool swapDimensions = displayRotation == DXGI_MODE_ROTATION_ROTATE90 || displayRotation == DXGI_MODE_ROTATION_ROTATE270;
	d3dRenderTargetSize.Width = swapDimensions ? outputSize.Height : outputSize.Width;
	d3dRenderTargetSize.Height = swapDimensions ? outputSize.Width : outputSize.Height;

	if (swapChain != nullptr)
	{
		// If the swap chain already exists, resize it.
		HRESULT hr = swapChain->ResizeBuffers(
			c_frameCount,
			lround(d3dRenderTargetSize.Width),
			lround(d3dRenderTargetSize.Height),
			DXGI_FORMAT_B8G8R8A8_UNORM,
			0
			);

		if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
		{
			// If the device was removed for any reason, a new device and swap chain will need to be created.
			deviceRemoved = true;

			// Do not continue execution of this method. DeviceResources will be destroyed and re-created.
			return;
		}
		else
		{
			DX::ThrowIfFailed(hr);
		}
	}
	else
	{
		// Otherwise, create a new one using the same adapter as the existing Direct3D device.
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};

		swapChainDesc.Width = lround(d3dRenderTargetSize.Width);	// Match the size of the window.
		swapChainDesc.Height = lround(d3dRenderTargetSize.Height);
		swapChainDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;			// This is the most common swap chain format.
		swapChainDesc.Stereo = false;
		swapChainDesc.SampleDesc.Count = 1;							// Don't use multi-sampling.
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = c_frameCount;					// Use triple-buffering to minimize latency.
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;	// All Windows Universal apps must use _FLIP_ SwapEffects
		swapChainDesc.Flags = 0;
		swapChainDesc.Scaling = DXGI_SCALING_NONE;
		swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;

		ComPtr<IDXGISwapChain1> swapChain;
		DX::ThrowIfFailed(
			dxgiFactory->CreateSwapChainForCoreWindow(
				commandQueue.Get(),
				reinterpret_cast<IUnknown*>(window.Get()),
				&swapChainDesc,
				nullptr,
				&swapChain
				)
			);

		DX::ThrowIfFailed(swapChain.As(&swapChain));
	}

	// Set the proper orientation for the swap chain, and generate
	// 3D matrix transformations for rendering to the rotated swap chain.
	// The 3D matrix is specified explicitly to avoid rounding errors.

	switch (displayRotation)
	{
	case DXGI_MODE_ROTATION_IDENTITY:
		orientationTransform3D = ScreenRotation::Rotation0;
		break;

	case DXGI_MODE_ROTATION_ROTATE90:
		orientationTransform3D = ScreenRotation::Rotation270;
		break;

	case DXGI_MODE_ROTATION_ROTATE180:
		orientationTransform3D = ScreenRotation::Rotation180;
		break;

	case DXGI_MODE_ROTATION_ROTATE270:
		orientationTransform3D = ScreenRotation::Rotation90;
		break;

	default:
		throw ref new FailureException();
	}

	DX::ThrowIfFailed(
		swapChain->SetRotation(displayRotation)
		);

	// Create a render target view of the swap chain back buffer.
	{
		D3D12_DESCRIPTOR_HEAP_DESC desc = {};
		desc.NumDescriptors = c_frameCount;
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		DX::ThrowIfFailed(d3dDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&rtvHeap)));
		rtvHeap->SetName(L"Render Target View Descriptor Heap");

		// All pending GPU work was already finished. Update the tracked fence values
		// to the last value signaled.
		for (UINT n = 0; n < c_frameCount; n++)
		{
			fenceValues[n] = fenceValues[currentFrame];
		}

		currentFrame = 0;
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvDescriptor(rtvHeap->GetCPUDescriptorHandleForHeapStart());
		rtvDescriptorSize = d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		for (UINT n = 0; n < c_frameCount; n++)
		{
			DX::ThrowIfFailed(swapChain->GetBuffer(n, IID_PPV_ARGS(&renderTargets[n])));
			d3dDevice->CreateRenderTargetView(renderTargets[n].Get(), nullptr, rtvDescriptor);
			rtvDescriptor.Offset(rtvDescriptorSize);

			WCHAR name[25];
			swprintf_s(name, L"Render Target %d", n);
			renderTargets[n]->SetName(name);
		}
	}

	// Set the 3D rendering viewport to target the entire window.
	screenViewport = { 0.0f, 0.0f, d3dRenderTargetSize.Width, d3dRenderTargetSize.Height, 0.0f, 1.0f };
}

// This method is called when the CoreWindow is created (or re-created).
void DX::DeviceResources::SetWindow(CoreWindow^ window)
{
	DisplayInformation^ currentDisplayInformation = DisplayInformation::GetForCurrentView();

	window = window;
	logicalSize = Windows::Foundation::Size(window->Bounds.Width, window->Bounds.Height);
	nativeOrientation = currentDisplayInformation->NativeOrientation;
	currentOrientation = currentDisplayInformation->CurrentOrientation;
	dpi = currentDisplayInformation->LogicalDpi;

	CreateWindowSizeDependentResources();
}

// This method is called in the event handler for the SizeChanged event.
void DX::DeviceResources::SetLogicalSize(Windows::Foundation::Size logicalSize)
{
	if (logicalSize != logicalSize)
	{
		logicalSize = logicalSize;
		CreateWindowSizeDependentResources();
	}
}

// This method is called in the event handler for the DpiChanged event.
void DX::DeviceResources::SetDpi(float dpi)
{
	if (dpi != dpi)
	{
		dpi = dpi;

		// When the display DPI changes, the logical size of the window (measured in Dips) also changes and needs to be updated.
		logicalSize = Windows::Foundation::Size(window->Bounds.Width, window->Bounds.Height);

		CreateWindowSizeDependentResources();
	}
}

// This method is called in the event handler for the OrientationChanged event.
void DX::DeviceResources::SetCurrentOrientation(DisplayOrientations currentOrientation)
{
	if (currentOrientation != currentOrientation)
	{
		currentOrientation = currentOrientation;
		CreateWindowSizeDependentResources();
	}
}

// This method is called in the event handler for the DisplayContentsInvalidated event.
void DX::DeviceResources::ValidateDevice()
{
	// The D3D Device is no longer valid if the default adapter changed since the device
	// was created or if the device has been removed.

	// First, get the LUID for the adapter from when the device was created.

	LUID previousAdapterLuid = d3dDevice->GetAdapterLuid();

	// Next, get the information for the current default adapter.

	ComPtr<IDXGIFactory2> currentFactory;
	DX::ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&currentFactory)));

	ComPtr<IDXGIAdapter1> currentDefaultAdapter;
	DX::ThrowIfFailed(currentFactory->EnumAdapters1(0, &currentDefaultAdapter));

	DXGI_ADAPTER_DESC currentDesc;
	DX::ThrowIfFailed(currentDefaultAdapter->GetDesc(&currentDesc));

	// If the adapter LUIDs don't match, or if the device reports that it has been removed,
	// a new D3D device must be created.

	if (previousAdapterLuid.LowPart != currentDesc.AdapterLuid.LowPart ||
		previousAdapterLuid.HighPart != currentDesc.AdapterLuid.HighPart ||
		FAILED(d3dDevice->GetDeviceRemovedReason()))
	{
		deviceRemoved = true;
	}
}

// Present the contents of the swap chain to the screen.
void DX::DeviceResources::Present()
{
	// The first argument instructs DXGI to block until VSync, putting the application
	// to sleep until the next VSync. This ensures we don't waste any cycles rendering
	// frames that will never be displayed to the screen.
	HRESULT hr = swapChain->Present(1, 0);

	// If the device was removed either by a disconnection or a driver upgrade, we 
	// must recreate all device resources.
	if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
	{
		deviceRemoved = true;
	}
	else
	{
		DX::ThrowIfFailed(hr);

		MoveToNextFrame();
	}
}

// Wait for pending GPU work to complete.
void DX::DeviceResources::WaitForGpu()
{
	// Schedule a Signal command in the queue.
	DX::ThrowIfFailed(commandQueue->Signal(fence.Get(), fenceValues[currentFrame]));

	// Wait until the fence has been crossed.
	DX::ThrowIfFailed(fence->SetEventOnCompletion(fenceValues[currentFrame], fenceEvent));
	WaitForSingleObjectEx(fenceEvent, INFINITE, FALSE);

	// Increment the fence value for the current frame.
	fenceValues[currentFrame]++;
}

// Prepare to render the next frame.
void DX::DeviceResources::MoveToNextFrame()
{
	// Schedule a Signal command in the queue.
	const UINT64 currentFenceValue = fenceValues[currentFrame];
	DX::ThrowIfFailed(commandQueue->Signal(fence.Get(), currentFenceValue));

	// Advance the frame index.
	currentFrame = (currentFrame + 1) % c_frameCount;

	// Check to see if the next frame is ready to start.
	if (fence->GetCompletedValue() < fenceValues[currentFrame])
	{
		DX::ThrowIfFailed(fence->SetEventOnCompletion(fenceValues[currentFrame], fenceEvent));
		WaitForSingleObjectEx(fenceEvent, INFINITE, FALSE);
	}

	// Set the fence value for the next frame.
	fenceValues[currentFrame] = currentFenceValue + 1;
}

// This method determines the rotation between the display device's native Orientation and the
// current display orientation.
DXGI_MODE_ROTATION DX::DeviceResources::ComputeDisplayRotation()
{
	DXGI_MODE_ROTATION rotation = DXGI_MODE_ROTATION_UNSPECIFIED;

	// Note: NativeOrientation can only be Landscape or Portrait even though
	// the DisplayOrientations enum has other values.
	switch (nativeOrientation)
	{
	case DisplayOrientations::Landscape:
		switch (currentOrientation)
		{
		case DisplayOrientations::Landscape:
			rotation = DXGI_MODE_ROTATION_IDENTITY;
			break;

		case DisplayOrientations::Portrait:
			rotation = DXGI_MODE_ROTATION_ROTATE270;
			break;

		case DisplayOrientations::LandscapeFlipped:
			rotation = DXGI_MODE_ROTATION_ROTATE180;
			break;

		case DisplayOrientations::PortraitFlipped:
			rotation = DXGI_MODE_ROTATION_ROTATE90;
			break;
		}
		break;

	case DisplayOrientations::Portrait:
		switch (currentOrientation)
		{
		case DisplayOrientations::Landscape:
			rotation = DXGI_MODE_ROTATION_ROTATE90;
			break;

		case DisplayOrientations::Portrait:
			rotation = DXGI_MODE_ROTATION_IDENTITY;
			break;

		case DisplayOrientations::LandscapeFlipped:
			rotation = DXGI_MODE_ROTATION_ROTATE270;
			break;

		case DisplayOrientations::PortraitFlipped:
			rotation = DXGI_MODE_ROTATION_ROTATE180;
			break;
		}
		break;
	}
	return rotation;
}