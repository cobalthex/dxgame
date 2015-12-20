#pragma once

#include "pch.hpp"

namespace DX
{
	static const UINT c_frameCount = 3;		// Use triple buffering.

	// Controls all the DirectX device resources.
	class DeviceResources
	{
	public:
		DeviceResources();
		void SetWindow(Windows::UI::Core::CoreWindow^ window);
		void SetLogicalSize(Windows::Foundation::Size logicalSize);
		void SetCurrentOrientation(Windows::Graphics::Display::DisplayOrientations currentOrientation);
		void SetDpi(float dpi);
		void ValidateDevice();
		void Present();
		void WaitForGpu();

		// Device Accessors.
		Windows::Foundation::Size	GetOutputSize() const				{ return outputSize; }
		Windows::Foundation::Size	GetLogicalSize() const				{ return logicalSize; }
		bool						IsDeviceRemoved() const				{ return deviceRemoved; }

		// D3D Accessors.
		ID3D12Device*				GetD3DDevice() const				{ return d3dDevice.Get(); }
		IDXGISwapChain3*			GetSwapChain() const				{ return swapChain.Get(); }
		ID3D12Resource*				GetRenderTarget() const				{ return renderTargets[currentFrame].Get(); }
		ID3D12CommandQueue*			GetCommandQueue() const				{ return commandQueue.Get(); }
		ID3D12CommandAllocator*		GetCommandAllocator() const			{ return commandAllocators[currentFrame].Get(); }
		D3D12_VIEWPORT				GetScreenViewport() const			{ return screenViewport; }
		DirectX::XMFLOAT4X4			GetOrientationTransform3D() const	{ return orientationTransform3D; }
		UINT						GetCurrentFrameIndex() const		{ return currentFrame; }

		CD3DX12_CPU_DESCRIPTOR_HANDLE GetRenderTargetView() const
		{
			return CD3DX12_CPU_DESCRIPTOR_HANDLE(rtvHeap->GetCPUDescriptorHandleForHeapStart(), currentFrame, rtvDescriptorSize);
		}

	private:
		void CreateDeviceIndependentResources();
		void CreateDeviceResources();
		void CreateWindowSizeDependentResources();
		void MoveToNextFrame();
		DXGI_MODE_ROTATION ComputeDisplayRotation();

		UINT											currentFrame;

		// Direct3D objects.
		Microsoft::WRL::ComPtr<ID3D12Device>			d3dDevice;
		Microsoft::WRL::ComPtr<IDXGIFactory4>			dxgiFactory;
		Microsoft::WRL::ComPtr<IDXGISwapChain3>			swapChain;
		Microsoft::WRL::ComPtr<ID3D12Resource>			renderTargets[c_frameCount];
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>	rtvHeap;
		UINT											rtvDescriptorSize;
		Microsoft::WRL::ComPtr<ID3D12CommandQueue>		commandQueue;
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator>	commandAllocators[c_frameCount];
		D3D12_VIEWPORT									screenViewport;
		bool											deviceRemoved;

		// CPU/GPU Synchronization.
		Microsoft::WRL::ComPtr<ID3D12Fence>				fence;
		UINT64											fenceValues[c_frameCount];
		HANDLE											fenceEvent;

		// Cached reference to the Window.
		Platform::Agile<Windows::UI::Core::CoreWindow>	window;

		// Cached device properties.
		Windows::Foundation::Size						d3dRenderTargetSize;
		Windows::Foundation::Size						outputSize;
		Windows::Foundation::Size						logicalSize;
		Windows::Graphics::Display::DisplayOrientations	nativeOrientation;
		Windows::Graphics::Display::DisplayOrientations	currentOrientation;
		float											dpi;

		// Transforms used for display orientation.
		DirectX::XMFLOAT4X4 orientationTransform3D;
	};
}
using DeviceResourcesPtr = std::shared_ptr<DX::DeviceResources>;