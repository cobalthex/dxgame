#include "Pch.hpp"
#include "DeviceResources.hpp"
#include "Engine/Common/PlatformHelpers.hpp"
#include "Shaders/ShaderStructures.hpp"

using namespace DirectX;
using namespace D2D1;
using namespace Microsoft::WRL;
using namespace Windows::Foundation;
using namespace Windows::Graphics::Display;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml::Controls;
using namespace Platform;

//Constants used to calculate screen rotations
namespace ScreenRotation
{
	//0-degree Z-rotation
	static const XMFLOAT4X4 Rotation0(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
		);

	//90-degree Z-rotation
	static const XMFLOAT4X4 Rotation90(
		0.0f, 1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
		);

	//180-degree Z-rotation
	static const XMFLOAT4X4 Rotation180(
		-1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
		);

	//270-degree Z-rotation
	static const XMFLOAT4X4 Rotation270(
		0.0f, -1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
		);
};

//Constructor for DeviceResources.
DeviceResources::DeviceResources() :
	screenViewport(),
	d3dFeatureLevel(D3D_FEATURE_LEVEL_11_1),
	d3dRenderTargetSize(),
	outputSize(),
	logicalSize(),
	nativeOrientation(DisplayOrientations::None),
	currentOrientation(DisplayOrientations::None),
	dpi(-1.0f),
	deviceNotify(nullptr),
	sampleSize(1),
	sampleQuality(0)
{
	CreateDeviceIndependentResources();
	CreateDeviceResources();
}

//Configures resources that don't depend on the Direct3D device.
void DeviceResources::CreateDeviceIndependentResources()
{
	//Initialize Direct2D resources.
	D2D1_FACTORY_OPTIONS options;
	ZeroMemory(&options, sizeof(D2D1_FACTORY_OPTIONS));

#if defined(_DEBUG)
	//If the project is in a debug build, enable Direct2D debugging via SDK Layers.
	options.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
#endif

	Sys::ThrowIfFailed(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory2), &options, &d2dFactory));
	Sys::ThrowIfFailed(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory2), &dwriteFactory));
	Sys::ThrowIfFailed(CoCreateInstance(CLSID_WICImagingFactory2, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&wicFactory)));
}

//Configures the Direct3D device, and stores handles to it and the device context.
void DeviceResources::CreateDeviceResources()
{
	//This flag adds support for surfaces with a different color channel ordering
	//than the API default. It is required for compatibility with Direct2D.
	UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

#if defined(_DEBUG)
	if (DX::SdkLayersAvailable())
	{
		//If the project is in a debug build, enable debugging via SDK Layers with this flag.
		creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
	}
#endif

	//This array defines the set of DirectX hardware feature levels this app will support.
	//Note the ordering should be preserved.
	//Don't forget to declare your application's minimum required feature level in its
	//description.  All applications are assumed to support 9.1 unless otherwise stated.
	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1
	};

	//Create the Direct3D 11 API device object and a corresponding context.
	ComPtr<ID3D11Device> device;
	ComPtr<ID3D11DeviceContext> context;

	HRESULT hr = D3D11CreateDevice(
		nullptr,					//Specify nullptr to use the default adapter.
		D3D_DRIVER_TYPE_HARDWARE,	//Create a device using the hardware graphics driver.
		0,							//Should be 0 unless the driver is D3D_DRIVER_TYPE_SOFTWARE.
		creationFlags,				//Set debug and Direct2D compatibility flags.
		featureLevels,				//List of feature levels this app can support.
		ARRAYSIZE(featureLevels),	//Size of the list above.
		D3D11_SDK_VERSION,			//Always set this to D3D11_SDK_VERSION for Windows Store apps.
		&device,					//Returns the Direct3D device created.
		&d3dFeatureLevel,			//Returns feature level of device created.
		&context					//Returns the device immediate context.
		);

	if (FAILED(hr))
	{
		//If the initialization fails, fall back to the WARP device.
		//For more information on WARP, see: 
		//http://go.microsoft.com/fwlink/?LinkId=286690
		Sys::ThrowIfFailed(
			D3D11CreateDevice(
				nullptr,
				D3D_DRIVER_TYPE_WARP, //Create a WARP device instead of a hardware device.
				0,
				creationFlags,
				featureLevels,
				ARRAYSIZE(featureLevels),
				D3D11_SDK_VERSION,
				&device,
				&d3dFeatureLevel,
				&context
				)
			);
	}

	//Store pointers to the Direct3D 11.1 API device and immediate context.
	Sys::ThrowIfFailed(device.As(&d3dDevice));
	Sys::ThrowIfFailed(context.As(&d3dContext));

	//Create the Direct2D device object and a corresponding context.
	ComPtr<IDXGIDevice3> dxgiDevice;
	Sys::ThrowIfFailed(d3dDevice.As(&dxgiDevice));

	Sys::ThrowIfFailed(d2dFactory->CreateDevice(dxgiDevice.Get(), &d2dDevice));
	Sys::ThrowIfFailed(d2dDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &d2dContext));
}

//These resources need to be recreated every time the window size is changed.
void DeviceResources::CreateWindowResources()
{
#if (WINAPI_FAMILY == WINAPI_FAMILY_PHONE_APP)
	//Windows Phone does not support resizing the swap chain, so clear it instead of resizing.
	swapChain = nullptr;
#endif
	ID3D11RenderTargetView* nullViews[] = { nullptr };
	d3dContext->OMSetRenderTargets(ARRAYSIZE(nullViews), nullViews, nullptr);
	d3dRenderTargetView = nullptr;
	d3dDepthStencilView = nullptr;
	d2dContext->SetTarget(nullptr);
	d2dTargetBitmap = nullptr;
	d3dContext->Flush();

	//Calculate the necessary render target size in pixels.
	outputSize.Width = Sys::ConvertDipsToPixels(logicalSize.Width, dpi);
	outputSize.Height = Sys::ConvertDipsToPixels(logicalSize.Height, dpi);

	//Prevent zero size DirectX content from being created.
	outputSize.Width = max(outputSize.Width, 1);
	outputSize.Height = max(outputSize.Height, 1);

	//The width and height of the swap chain must be based on the window's
	//natively-oriented width and height. If the window is not in the native
	//orientation, the dimensions must be reversed.
	DXGI_MODE_ROTATION displayRotation = ComputeDisplayRotation();

	bool swapDimensions = displayRotation == DXGI_MODE_ROTATION_ROTATE90 || displayRotation == DXGI_MODE_ROTATION_ROTATE270;
	d3dRenderTargetSize.Width = swapDimensions ? outputSize.Height : outputSize.Width;
	d3dRenderTargetSize.Height = swapDimensions ? outputSize.Width : outputSize.Height;

	backBuffer.ReleaseAndGetAddressOf();

	if (swapChain != nullptr)
	{
		//If the swap chain already exists, resize it.
		HRESULT hr = swapChain->ResizeBuffers(
			2, //Double-buffered swap chain.
			lround(d3dRenderTargetSize.Width),
			lround(d3dRenderTargetSize.Height),
			DXGI_FORMAT_B8G8R8A8_UNORM,
			0
			);

		if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
		{
			//If the device was removed for any reason, a new device and swap chain will need to be created.
			HandleDeviceLost();

			//Everything is set up now. Do not continue execution of this method. HandleDeviceLost will reenter this method 
			//and correctly set up the new device.
			return;
		}
		else
		{
			Sys::ThrowIfFailed(hr);
		}
	}
	else
	{
		if (enableMultisampling)
		{
			//find supported multi-sampling
			for (unsigned s = D3D11_MAX_MULTISAMPLE_SAMPLE_COUNT; s > 0; s--) //minimum 1
			{
				UINT q;
				HRESULT test = d3dDevice->CheckMultisampleQualityLevels(DXGI_FORMAT_B8G8R8A8_UNORM, s, &q);

				if (SUCCEEDED(test) && (q > 0))
				{
					sampleSize = s;
					sampleQuality = q - 1;
					break;
				}
			}
		}

		//Otherwise, create a new one using the same adapter as the existing Direct3D device.
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = { 0 };

		swapChainDesc.Width = lround(d3dRenderTargetSize.Width); //Match the size of the window.
		swapChainDesc.Height = lround(d3dRenderTargetSize.Height);
		swapChainDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM; //This is the most common swap chain format.
		swapChainDesc.Stereo = false;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = 2; //Use double-buffering to minimize latency.
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL; //All Windows Store apps must use this SwapEffect.
		swapChainDesc.Flags = 0;
		swapChainDesc.Scaling = DXGI_SCALING_NONE;
		swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;

		//This sequence obtains the DXGI factory that was used to create the Direct3D device above.
		ComPtr<IDXGIDevice3> dxgiDevice;
		Sys::ThrowIfFailed(d3dDevice.As(&dxgiDevice));

		ComPtr<IDXGIAdapter> dxgiAdapter;
		Sys::ThrowIfFailed(dxgiDevice->GetAdapter(&dxgiAdapter));

		ComPtr<IDXGIFactory2> dxgiFactory;
		Sys::ThrowIfFailed(dxgiAdapter->GetParent(IID_PPV_ARGS(&dxgiFactory)));

		Sys::ThrowIfFailed(
			dxgiFactory->CreateSwapChainForCoreWindow(
				d3dDevice.Get(),
				reinterpret_cast<IUnknown*>(window.Get()),
				&swapChainDesc,
				nullptr,
				&swapChain
				)
			);

		//Ensure that DXGI does not queue more than one frame at a time. This both reduces latency and
		//ensures that the application will only render after each VSync, minimizing power consumption.
		Sys::ThrowIfFailed(dxgiDevice->SetMaximumFrameLatency(1));
	}

	//Set the proper orientation for the swap chain, and generate 2D and
	//3D matrix transformations for rendering to the rotated swap chain.
	//Note the rotation angle for the 2D and 3D transforms are different.
	//This is due to the difference in coordinate spaces.  Additionally,
	//the 3D matrix is specified explicitly to avoid rounding errors.

	switch (displayRotation)
	{
	case DXGI_MODE_ROTATION_IDENTITY:
		orientationTransform2D = Matrix3x2F::Identity();
		orientationTransform3D = ScreenRotation::Rotation0;
		break;

	case DXGI_MODE_ROTATION_ROTATE90:
		orientationTransform2D =
			Matrix3x2F::Rotation(90.0f) *
			Matrix3x2F::Translation(logicalSize.Height, 0.0f);
		orientationTransform3D = ScreenRotation::Rotation270;
		break;

	case DXGI_MODE_ROTATION_ROTATE180:
		orientationTransform2D =
			Matrix3x2F::Rotation(180.0f) *
			Matrix3x2F::Translation(logicalSize.Width, logicalSize.Height);
		orientationTransform3D = ScreenRotation::Rotation180;
		break;

	case DXGI_MODE_ROTATION_ROTATE270:
		orientationTransform3D = ScreenRotation::Rotation90;
		break;

	default:
		throw ref new FailureException();
	}

	Sys::ThrowIfFailed(swapChain->SetRotation(displayRotation));

	Sys::ThrowIfFailed(swapChain->GetBuffer(0, IID_PPV_ARGS(backBuffer.GetAddressOf())));

	//create a special multi-sampled surface
	D3D11_TEXTURE2D_DESC offScreenSurfaceDesc;
	ZeroMemory(&offScreenSurfaceDesc, sizeof(D3D11_TEXTURE2D_DESC));

	offScreenSurfaceDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	offScreenSurfaceDesc.Width = lround(d3dRenderTargetSize.Width);
	offScreenSurfaceDesc.Height = lround(d3dRenderTargetSize.Height);
	offScreenSurfaceDesc.BindFlags = D3D11_BIND_RENDER_TARGET;
	offScreenSurfaceDesc.MipLevels = 1;
	offScreenSurfaceDesc.ArraySize = 1;
	offScreenSurfaceDesc.SampleDesc.Count = sampleSize;
	offScreenSurfaceDesc.SampleDesc.Quality = sampleQuality;

	// Create a surface that's multisampled
	Sys::ThrowIfFailed(
		d3dDevice->CreateTexture2D(
			&offScreenSurfaceDesc,
			nullptr,
			&offScreenSurface)
		);

	CD3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc(D3D11_RTV_DIMENSION_TEXTURE2DMS);
	Sys::ThrowIfFailed(
		d3dDevice->CreateRenderTargetView(
			offScreenSurface.Get(),
			&renderTargetViewDesc,
			&d3dRenderTargetView
			)
		);

	//Create a depth stencil view for use with 3D rendering if needed.
	CD3D11_TEXTURE2D_DESC depthStencilDesc(
		DXGI_FORMAT_D24_UNORM_S8_UINT,
		lround(d3dRenderTargetSize.Width),
		lround(d3dRenderTargetSize.Height),
		1, //This depth stencil view has only one texture.
		1, //Use a single mipmap level.
		D3D11_BIND_DEPTH_STENCIL,
		D3D11_USAGE_DEFAULT,
		0,
		sampleSize,
		sampleQuality,
		0
		);

	ComPtr<ID3D11Texture2D> depthStencil;
	Sys::ThrowIfFailed(
		d3dDevice->CreateTexture2D(
			&depthStencilDesc,
			nullptr,
			&depthStencil
			)
		);

	CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(D3D11_DSV_DIMENSION_TEXTURE2DMS);
	Sys::ThrowIfFailed(
		d3dDevice->CreateDepthStencilView(
			depthStencil.Get(),
			&depthStencilViewDesc,
			&d3dDepthStencilView
			)
		);

	//Set the 3D rendering viewport to target the entire window.
	screenViewport = CD3D11_VIEWPORT(
		0.0f,
		0.0f,
		d3dRenderTargetSize.Width,
		d3dRenderTargetSize.Height
		);

	d3dContext->RSSetViewports(1, &screenViewport);
	
	//Create a Direct2D target bitmap associated with the swap chain back buffer and set it as the current target.
	D2D1_BITMAP_PROPERTIES1 bitmapProperties = D2D1::BitmapProperties1(
		D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW, 
		D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
		dpi,
		dpi
	);
	
	ComPtr<IDXGISurface2> dxgiBackBuffer;
	//Sys::ThrowIfFailed(swapChain->GetBuffer(0, IID_PPV_ARGS(&dxgiBackBuffer))); //offscreen surface must be used when multisampling
	Sys::ThrowIfFailed(offScreenSurface.Get()->QueryInterface(IID_PPV_ARGS(&dxgiBackBuffer)));
	Sys::ThrowIfFailed(d2dContext->CreateBitmapFromDxgiSurface(
		dxgiBackBuffer.Get(),
		&bitmapProperties,
		&d2dTargetBitmap)
	);
	d2dContext->SetTarget(d2dTargetBitmap.Get());

	//Below are some default states. If the active state is modified, it's the responsibility of whatever set the state to unset it

	d3dContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//create default alpha blending blend state
	D3D11_BLEND_DESC blDesc;
	ZeroMemory(&blDesc, sizeof(D3D11_BLEND_DESC));
	blDesc.RenderTarget[0].BlendEnable = TRUE;

	blDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;

	blDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;

	blDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	blDesc.IndependentBlendEnable = FALSE;
	blDesc.AlphaToCoverageEnable = FALSE;

	ComPtr<ID3D11BlendState> blState;
	Sys::ThrowIfFailed(d3dDevice->CreateBlendState(&blDesc, &blState));
	d3dContext->OMSetBlendState(blState.Get(), 0, 0xffffffff);

	//default sampler
	D3D11_SAMPLER_DESC sampDesc;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = -FLT_MAX;
	sampDesc.MaxLOD = FLT_MAX;
	sampDesc.MaxAnisotropy = 16;
	sampDesc.MipLODBias = 0;
	ComPtr<ID3D11SamplerState> sampState;
	d3dDevice->CreateSamplerState(&sampDesc, &sampState);
	d3dContext->PSSetSamplers(0, 1, sampState.GetAddressOf());

	//default rasterizer
	D3D11_RASTERIZER_DESC rzDesc;
	ZeroMemory(&rzDesc, sizeof(D3D11_RASTERIZER_DESC));

	rzDesc.FillMode = D3D11_FILL_SOLID;
	rzDesc.CullMode = D3D11_CULL_BACK;
	rzDesc.AntialiasedLineEnable = true;
	rzDesc.MultisampleEnable = true;

	ComPtr<ID3D11RasterizerState> rzState;
	//Sys::ThrowIfFailed(d3dDevice->CreateRasterizerState(&rzDesc, &rzState));
	//d3dContext->RSSetState(rzState.Get());
}

//This method is called when the CoreWindow is created (or re-created).
void DeviceResources::SetWindow(CoreWindow^ Window)
{
	DisplayInformation^ currentDisplayInformation = DisplayInformation::GetForCurrentView();

	window = Window;
	logicalSize = Windows::Foundation::Size(window->Bounds.Width, window->Bounds.Height);
	nativeOrientation = currentDisplayInformation->NativeOrientation;
	currentOrientation = currentDisplayInformation->CurrentOrientation;
	dpi = currentDisplayInformation->LogicalDpi;
	d2dContext->SetDpi(dpi, dpi);

	CreateWindowResources();
}

//This method is called in the event handler for the SizeChanged event.
void DeviceResources::SetLogicalSize(Windows::Foundation::Size LogicalSize)
{
	if (logicalSize != LogicalSize)
	{
		logicalSize = LogicalSize;
		CreateWindowResources();
	}
}

//This method is called in the event handler for the DpiChanged event.
void DeviceResources::SetDpi(float Dpi)
{
	if (dpi != Dpi)
	{
		dpi = Dpi;
		d2dContext->SetDpi(dpi, dpi);

		//When the display DPI changes, the logical size of the window (measured in Dips) also changes and needs to be updated.
		logicalSize = Windows::Foundation::Size(window->Bounds.Width, window->Bounds.Height);

		CreateWindowResources();
	}
}

//This method is called in the event handler for the OrientationChanged event.
void DeviceResources::SetCurrentOrientation(DisplayOrientations CurrentOrientation)
{
	if (currentOrientation != CurrentOrientation)
	{
		currentOrientation = CurrentOrientation;
		CreateWindowResources();
	}
}

//This method is called in the event handler for the DisplayContentsInvalidated event.
void DeviceResources::ValidateDevice()
{
	//The D3D Device is no longer valid if the default adapter changed since the device
	//was created or if the device has been removed.

	//First, get the information for the default adapter from when the device was created.

	ComPtr<IDXGIDevice3> dxgiDevice;
	Sys::ThrowIfFailed(d3dDevice.As(&dxgiDevice));

	ComPtr<IDXGIAdapter> deviceAdapter;
	Sys::ThrowIfFailed(dxgiDevice->GetAdapter(&deviceAdapter));

	ComPtr<IDXGIFactory2> deviceFactory;
	Sys::ThrowIfFailed(deviceAdapter->GetParent(IID_PPV_ARGS(&deviceFactory)));

	ComPtr<IDXGIAdapter1> previousDefaultAdapter;
	Sys::ThrowIfFailed(deviceFactory->EnumAdapters1(0, &previousDefaultAdapter));

	DXGI_ADAPTER_DESC previousDesc;
	Sys::ThrowIfFailed(previousDefaultAdapter->GetDesc(&previousDesc));

	//Next, get the information for the current default adapter.

	ComPtr<IDXGIFactory2> currentFactory;
	Sys::ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&currentFactory)));

	ComPtr<IDXGIAdapter1> currentDefaultAdapter;
	Sys::ThrowIfFailed(currentFactory->EnumAdapters1(0, &currentDefaultAdapter));

	DXGI_ADAPTER_DESC currentDesc;
	Sys::ThrowIfFailed(currentDefaultAdapter->GetDesc(&currentDesc));

	//If the adapter LUIDs don't match, or if the device reports that it has been removed,
	//a new D3D device must be created.

	if (previousDesc.AdapterLuid.LowPart != currentDesc.AdapterLuid.LowPart ||
		previousDesc.AdapterLuid.HighPart != currentDesc.AdapterLuid.HighPart ||
		FAILED(d3dDevice->GetDeviceRemovedReason()))
	{
		//Release references to resources related to the old device.
		dxgiDevice = nullptr;
		deviceAdapter = nullptr;
		deviceFactory = nullptr;
		previousDefaultAdapter = nullptr;

		//Create a new device and swap chain.
		HandleDeviceLost();
	}
}

//Recreate all device resources and set them back to the current state.
void DeviceResources::HandleDeviceLost()
{
	swapChain = nullptr;

	if (deviceNotify != nullptr)
		deviceNotify->OnDeviceLost();

	CreateDeviceResources();
	d2dContext->SetDpi(dpi, dpi);
	CreateWindowResources();

	if (deviceNotify != nullptr)
		deviceNotify->OnDeviceRestored();
}

//Register our DeviceNotify to be informed on device lost and creation.
void DeviceResources::RegisterDeviceNotify(IDeviceNotify* DeviceNotify)
{
	deviceNotify = DeviceNotify;
}

//Call this method when the app suspends. It provides a hint to the driver that the app 
//is entering an idle state and that temporary buffers can be reclaimed for use by other apps.
void DeviceResources::Trim()
{
	ComPtr<IDXGIDevice3> dxgiDevice;
	d3dDevice.As(&dxgiDevice);

	dxgiDevice->Trim();
}

//Present the contents of the swap chain to the screen.
void DeviceResources::Present()
{
	//if multisampling is used, update the back buffer correctly
	if (sampleSize > 1)
	{
		unsigned int sub = D3D11CalcSubresource(0, 0, 1);

		d3dContext->ResolveSubresource(
			backBuffer.Get(),
			sub,
			offScreenSurface.Get(),
			sub,
			DXGI_FORMAT_B8G8R8A8_UNORM
			);
	}

	//The first argument instructs DXGI to block until VSync, putting the application
	//to sleep until the next VSync. This ensures we don't waste any cycles rendering
	//frames that will never be displayed to the screen.
	HRESULT hr = swapChain->Present(1, 0);

	//Discard the contents of the render target.
	//This is a valid operation only when the existing contents will be entirely
	//overwritten. If dirty or scroll rects are used, this call should be removed.
	d3dContext->DiscardView(d3dRenderTargetView.Get());

	//Discard the contents of the depth stencil.
	d3dContext->DiscardView(d3dDepthStencilView.Get());

	//If the device was removed either by a disconnection or a driver upgrade, we 
	//must recreate all device resources.
	if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
		HandleDeviceLost();
	else
		Sys::ThrowIfFailed(hr);
}

//This method determines the rotation between the display device's native Orientation and the
//current display orientation.
DXGI_MODE_ROTATION DeviceResources::ComputeDisplayRotation()
{
	DXGI_MODE_ROTATION rotation = DXGI_MODE_ROTATION_UNSPECIFIED;

	//Note: NativeOrientation can only be Landscape or Portrait even though
	//the DisplayOrientations enum has other values.
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