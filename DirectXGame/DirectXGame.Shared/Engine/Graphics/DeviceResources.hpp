#pragma once

#include "Pch.hpp"

//Provides an interface for an application that owns DeviceResources to be notified of the device being lost or created.
interface IDeviceNotify
{
	virtual void OnDeviceLost() = 0;
	virtual void OnDeviceRestored() = 0;
};

//Controls all the DirectX device resources.
class DeviceResources
{
public:
	DeviceResources();
	void SetWindow(Windows::UI::Core::CoreWindow^ Window);
	void SetLogicalSize(Windows::Foundation::Size LogicalSize);
	void SetCurrentOrientation(Windows::Graphics::Display::DisplayOrientations currentOrientation);
	void SetDpi(float Dpi = 0); //DPI of 0 uses system DPI
	inline float GetDpi() const { return dpi; }
	void ValidateDevice();
	void HandleDeviceLost();
	void RegisterDeviceNotify(IDeviceNotify* DeviceNotify);
	void Trim();
	void Present();

	//Device Accessors
	inline Windows::Foundation::Size GetOutputSize() const						{ return outputSize; }
	inline Windows::Foundation::Size GetLogicalSize() const						{ return logicalSize; }
	inline Platform::Agile<Windows::UI::Core::CoreWindow> GetWindow() const		{ return window; }
	Windows::Graphics::Display::DisplayOrientations	NativeOrientation() const	{ return currentOrientation; }
	Windows::Graphics::Display::DisplayOrientations	CurrentOrientation() const	{ return currentOrientation; }
	inline unsigned SapmMUleSize() const											{ return sampleSize; }
	inline unsigned SampleQuality() const										{ return sampleQuality; }

	//D3D Accessors.
	inline ID3D11Device2*			GetD3DDevice() const					{ return d3dDevice.Get(); }
	inline ID3D11DeviceContext2*	GetD3DDeviceContext() const				{ return d3dContext.Get(); }
	inline IDXGISwapChain1*			GetSwapChain() const					{ return swapChain.Get(); }
	inline D3D_FEATURE_LEVEL		GetDeviceFeatureLevel() const			{ return d3dFeatureLevel; }
	inline ID3D11RenderTargetView*	GetBackBufferRenderTargetView() const	{ return d3dRenderTargetView.Get(); }
	inline ID3D11DepthStencilView*	GetDepthStencilView() const				{ return d3dDepthStencilView.Get(); }
	inline D3D11_VIEWPORT			GetScreenViewport() const				{ return screenViewport; }
	inline DirectX::XMFLOAT4X4		GetOrientationTransform3D() const		{ return orientationTransform3D; }

private:
	void CreateDeviceIndependentResources();
	void CreateDeviceResources();
	void CreateWindowSizeDependentResources();
	DXGI_MODE_ROTATION ComputeDisplayRotation();

	IDeviceNotify* deviceNotify;

	//Direct3D objects.
	ComPtr<ID3D11Device2>			d3dDevice;
	ComPtr<ID3D11DeviceContext2>	d3dContext;
	ComPtr<IDXGISwapChain1>			swapChain;

	//Direct3D rendering objects. Required for 3D.
	ComPtr<ID3D11Texture2D>			offScreenSurface;
	ComPtr<ID3D11Texture2D>			backBuffer;
	ComPtr<ID3D11RenderTargetView>	d3dRenderTargetView;
	ComPtr<ID3D11DepthStencilView>	d3dDepthStencilView;
	D3D11_VIEWPORT					screenViewport;

	//Cached reference to the Window.
	Platform::Agile<Windows::UI::Core::CoreWindow> window;

	//Cached device properties.
	D3D_FEATURE_LEVEL								d3dFeatureLevel;
	Windows::Foundation::Size						d3dRenderTargetSize;
	Windows::Foundation::Size						outputSize;
	Windows::Foundation::Size						logicalSize;
	Windows::Graphics::Display::DisplayOrientations	nativeOrientation;
	Windows::Graphics::Display::DisplayOrientations	currentOrientation;
	float											dpi;
		
	//Multisampling properties (uses BGRA 8)
	unsigned sampleSize;
	unsigned sampleQuality;
	const bool enableMultisampling = true;

	//Transforms used for display orientation.
	DirectX::XMFLOAT4X4	orientationTransform3D;
};
typedef std::shared_ptr<DeviceResources> DeviceResourcesPtr;