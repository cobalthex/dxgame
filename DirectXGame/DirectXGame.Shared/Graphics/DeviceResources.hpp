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

	//Helper for managing assets.

	//Device Accessor.sM
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

	//D2D Accessors.
	inline ID2D1Factory2*		GetD2DFactory() const				{ return d2dFactory.Get(); }
	inline ID2D1Device1*		GetD2DDevice() const				{ return d2dDevice.Get(); }
	inline ID2D1DeviceContext1*	GetD2DDeviceContext() const			{ return d2dContext.Get(); }
	inline ID2D1Bitmap1*		GetD2DTargetBitmap() const			{ return d2dTargetBitmap.Get(); }
	inline IDWriteFactory2*		GetDWriteFactory() const			{ return dwriteFactory.Get();	 }
	inline IWICImagingFactory2*	GetWicImagingFactory() const		{ return wicFactory.Get(); }
	inline D2D1::Matrix3x2F		GetOrientationTransform2D() const	{ return orientationTransform2D; }

private:
	void CreateDeviceIndependentResources();
	void CreateDeviceResources();
	void CreateWindowSizeDependentResources();
	DXGI_MODE_ROTATION ComputeDisplayRotation();

	//Direct3D objects.
	Microsoft::WRL::ComPtr<ID3D11Device2>			d3dDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext2>	d3dContext;
	Microsoft::WRL::ComPtr<IDXGISwapChain1>			swapChain;

	//Direct3D rendering objects. Required for 3D.
	Microsoft::WRL::ComPtr<ID3D11Texture2D>			offScreenSurface;
	Microsoft::WRL::ComPtr<ID3D11Texture2D>			backBuffer;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>	d3dRenderTargetView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>	d3dDepthStencilView;
	D3D11_VIEWPORT									screenViewport;

	//Direct2D drawing components.
	Microsoft::WRL::ComPtr<ID2D1Factory2>		d2dFactory;
	Microsoft::WRL::ComPtr<ID2D1Device1>		d2dDevice;
	Microsoft::WRL::ComPtr<ID2D1DeviceContext1>	d2dContext;
	Microsoft::WRL::ComPtr<ID2D1Bitmap1>		d2dTargetBitmap;

	//DirectWrite drawing components.
	Microsoft::WRL::ComPtr<IDWriteFactory2>		dwriteFactory;
	Microsoft::WRL::ComPtr<IWICImagingFactory2>	wicFactory;

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

	//Transforms used for display orientation.
	D2D1::Matrix3x2F	orientationTransform2D;
	DirectX::XMFLOAT4X4	orientationTransform3D;

	//The IDeviceNotify can be held directly as it owns the DeviceResources.
	IDeviceNotify* deviceNotify;
};
typedef std::shared_ptr<DeviceResources> DeviceResourcesPtr;