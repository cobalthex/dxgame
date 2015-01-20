#include "pch.hpp"
#include "DirectXGameMain.hpp"
#include "Common\DirectXHelper.hpp"

using namespace DirectXGame;

using namespace Windows::Foundation;
using namespace Windows::System::Threading;
using namespace Concurrency;

// Loads and initializes application assets when the application is loaded.
DirectXGameMain::DirectXGameMain(const std::shared_ptr<DX::DeviceResources>& DeviceResources) :
	deviceResources(DeviceResources)
{
	// Register to be notified if the Device is lost or recreated
	deviceResources->RegisterDeviceNotify(this);

	// TODO: Replace this with your app's content initialization.
	sceneRenderer = std::unique_ptr<Sample3DSceneRenderer>(new Sample3DSceneRenderer(deviceResources));
	fpsTextRenderer = std::unique_ptr<SampleFpsTextRenderer>(new SampleFpsTextRenderer(deviceResources));
	textRenderer = std::make_unique<TextRenderer>(deviceResources);

	tlParams.maxWidth = 200;
	tlParams.maxHeight = 50;
	tlParams.textFormat = TextLayoutParams::CreateTextFormat(deviceResources, L"Segoe UI", 32);
	DX::ThrowIfFailed(deviceResources->GetD2DDeviceContext()->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Gold), &brush));

	// Set the timer settings to other than the default variable timestep mode.
	// e.g. for 60 FPS fixed timestep update logic, call:
	/*
	timer.SetFixedTimeStep(true);
	timer.SetTargetElapsedSeconds(1.0 / 60);
	*/
}

DirectXGameMain::~DirectXGameMain()
{
	deviceResources->RegisterDeviceNotify(nullptr);
}

// Updates application state when the window size changes (e.g. device orientation change)
void DirectXGameMain::CreateWindowSizeDependentResources() 
{
	// TODO: Replace this with the size-dependent initialization of your app's content.
	sceneRenderer->CreateWindowSizeDependentResources();
}

// Updates the application state once per frame.
void DirectXGameMain::Update() 
{
	// Update scene objects.
	timer.Tick([&]()
	{
		// TODO: Replace this with your app's content update functions.
		sceneRenderer->Update(timer);
		fpsTextRenderer->Update(timer);
	});
}

// Renders the current frame according to the current application state.
// Returns true if the frame was rendered and is ready to be displayed.
bool DirectXGameMain::Render() 
{
	// Don't try to render anything before the first Update.
	if (timer.GetFrameCount() == 0)
	{
		return false;
	}

	auto context = deviceResources->GetD3DDeviceContext();

	// Reset the viewport to target the whole screen.
	auto viewport = deviceResources->GetScreenViewport();
	context->RSSetViewports(1, &viewport);

	// Reset render targets to the screen.
	ID3D11RenderTargetView *const targets[1] = { deviceResources->GetBackBufferRenderTargetView() };
	context->OMSetRenderTargets(1, targets, deviceResources->GetDepthStencilView());

	// Clear the back buffer and depth stencil view.
	context->ClearRenderTargetView(deviceResources->GetBackBufferRenderTargetView(), DirectX::Colors::CornflowerBlue);
	context->ClearDepthStencilView(deviceResources->GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// Render the scene objects.
	// TODO: Replace this with your app's content rendering functions.
	sceneRenderer->Render();
	fpsTextRenderer->Render();

	textRenderer->Render(L"Test", tlParams, brush, D2D1::Matrix3x2F::Identity());

	return true;
}

// Notifies renderers that device resources need to be released.
void DirectXGameMain::OnDeviceLost()
{
	sceneRenderer->ReleaseDeviceDependentResources();
	fpsTextRenderer->ReleaseDeviceDependentResources();
}

// Notifies renderers that device resources may now be recreated.
void DirectXGameMain::OnDeviceRestored()
{
	sceneRenderer->CreateDeviceDependentResources();
	fpsTextRenderer->CreateDeviceDependentResources();
	CreateWindowSizeDependentResources();
}
