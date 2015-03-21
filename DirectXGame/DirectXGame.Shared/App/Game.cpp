﻿#include "pch.hpp"
#include "Game.hpp"
#include "Common/PlatformHelpers.hpp"

using namespace DirectXGame;

using namespace Windows::Foundation;
using namespace Windows::System::Threading;
using namespace Concurrency;

//Loads and initializes application assets when the application is loaded.
Game::Game(const std::shared_ptr<DeviceResources>& DeviceResources) :
	deviceResources(DeviceResources)
{
	//Register to be notified if the Device is lost or recreated
	deviceResources->RegisterDeviceNotify(this);

	//TODO: Replace this with your app's content initialization.
	scene = std::unique_ptr<TestScene>(new TestScene(deviceResources));
	fpsTextRenderer = std::unique_ptr<FpsRenderer>(new FpsRenderer(deviceResources));
	
	//Set the timer settings to other than the default variable timestep mode.
	//e.g. for 60 FPS fixed timestep update logic, call:
	/*
	timer.SetFixedTimeStep(true);
	timer.SetTargetElapsedSeconds(1.0 / 60);
	*/
}

Game::~Game()
{
	deviceResources->RegisterDeviceNotify(nullptr);
}

//Updates application state when the window size changes (e.g. device orientation change)
void Game::CreateWindowSizeDependentResources()
{
	//TODO: Replace this with the size-dependent initialization of your app's content.
	scene->CreateWindowSizeDependentResources();
}

//Updates the application state once per frame.
void Game::Update()
{
	//Update scene objects.
	timer.Tick([&]()
	{
		//TODO: Replace this with your app's content update functions.
		scene->Update(timer);
		fpsTextRenderer->Update(timer);
	});
}

//Renders the current frame according to the current application state.
//Returns true if the frame was rendered and is ready to be displayed.
bool Game::Render()
{
	//Don't try to render anything before the first Update.
	if (timer.GetFrameCount() == 0)
		return false;

	auto context = deviceResources->GetD3DDeviceContext();

	//Reset the viewport to target the whole screen
	auto viewport = deviceResources->GetScreenViewport();
	context->RSSetViewports(1, &viewport);

	//Reset render targets to the screen
	ID3D11RenderTargetView *const targets[1] = { deviceResources->GetBackBufferRenderTargetView() };
	context->OMSetRenderTargets(1, targets, deviceResources->GetDepthStencilView());

	//Clear the back buffer and depth stencil view
	context->ClearRenderTargetView(deviceResources->GetBackBufferRenderTargetView(), DirectX::Colors::CornflowerBlue);
	context->ClearDepthStencilView(deviceResources->GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	//Render the scene objects.
	//TODO: Replace this with your app's content rendering functions.
	scene->Render();
	fpsTextRenderer->Render();

	return true;
}

//Notifies renderers that device resources need to be released.
void Game::OnDeviceLost()
{
	scene->ReleaseDeviceDependentResources();
	fpsTextRenderer->ReleaseDeviceDependentResources();
}

//Notifies renderers that device resources may now be recreated.
void Game::OnDeviceRestored()
{
	scene->CreateDeviceDependentResources();
	fpsTextRenderer->CreateDeviceDependentResources();
	CreateWindowSizeDependentResources();
}
