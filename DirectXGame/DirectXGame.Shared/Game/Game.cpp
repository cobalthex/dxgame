#include "pch.hpp"
#include "Game.hpp"
#include "Engine/Common/PlatformHelpers.hpp"
#include "GameComponent.hpp"

#include "Engine/Graphics/Text/FpsDisplay.hpp"

using namespace Windows::Foundation;
using namespace Windows::System::Threading;
using namespace Concurrency;

//Loads and initializes application assets when the application is loaded.
Game::Game(const std::shared_ptr<DeviceResources>& DeviceResources) :
	deviceResources(DeviceResources), shaderCache(DeviceResources), FpsDisplay(DeviceResources, shaderCache.Load<Shaders::TextShader>(ShaderType::Text))
{
	//Register to be notified if the Device is lost or recreated
	deviceResources->RegisterDeviceNotify(this);

	//TODO: Replace this with your app's content initialization.
	//components.push_back(TestScene(*this, deviceResources));
	
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
void Game::CreateWindowResources(Windows::UI::Core::CoreWindow^ Window)
{
	for (auto& c : components)
		c->CreateWindowResources(Window);

	FpsDisplay.CreateWindowResources(Window);
}

//Updates the application state once per frame.
void Game::Update()
{
	//Update scene objects.
	timer.Tick([&]()
	{
		for (auto& c : components)
		{
			if (c->isEnabled)
				c->Update(timer);
		}
	});
}

//Renders the current frame according to the current application state.
//Returns true if the frame was rendered and is ready to be displayed.
bool Game::Draw()
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

	for (auto& c : components)
	{
		if (c->isVisible)
			c->Draw(timer);
	}

	FpsDisplay.Draw(timer);

	return true;
}

//Notifies renderers that device resources need to be released.
void Game::OnDeviceLost()
{
	for (auto& c : components)
		c->ReleaseDeviceResources();

	FpsDisplay.ReleaseDeviceResources();
}

//Notifies renderers that device resources may now be recreated.
void Game::OnDeviceRestored()
{
	//recreate shaders?


	for (auto& c : components)
		c->CreateDeviceResources();
	FpsDisplay.CreateDeviceResources();

	CreateWindowResources(deviceResources->GetWindow().Get());
}

void Game::PushComponent(const std::shared_ptr<GameComponent>& GComponent)
{
	components.push_back(GComponent);
	GComponent->CreateDeviceResources();
	GComponent->CreateWindowResources(deviceResources->GetWindow().Get());
}

std::shared_ptr<GameComponent> Game::PopComponent()
{
	if (components.size() < 1)
		return nullptr;

	auto gcomp = components.back();
	components.pop_back();
	gcomp->ReleaseDeviceResources();
	return gcomp;
}