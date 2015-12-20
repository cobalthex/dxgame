#include "pch.hpp"
#include "Dx12Main.hpp"
#include "Graphics\DirectXHelper.hpp"

using namespace Dx12;
using namespace Windows::Foundation;
using namespace Windows::System::Threading;
using namespace Concurrency;

// The DirectX 12 Application template is documented at http://go.microsoft.com/fwlink/?LinkID=613670&clcid=0x409

// Loads and initializes application assets when the application is loaded.
Dx12Main::Dx12Main()
{
	// TODO: Change the timer settings if you want something other than the default variable timestep mode.
	// e.g. for 60 FPS fixed timestep update logic, call:
	/*
	timer.SetFixedTimeStep(true);
	timer.SetTargetElapsedSeconds(1.0 / 60);
	*/
}

// Creates and initializes the renderers.
void Dx12Main::CreateRenderers(const std::shared_ptr<DX::DeviceResources>& deviceResources)
{
	// TODO: Replace this with your app's content initialization.
	sceneRenderer = std::unique_ptr<Scene>(new Scene(deviceResources));

	OnWindowSizeChanged();
}

// Updates the application state once per frame.
void Dx12Main::Update()
{
	// Update scene objects.
	timer.Tick([&]()
	{
		// TODO: Replace this with your app's content update functions.
		sceneRenderer->Update(timer);
	});
}

// Renders the current frame according to the current application state.
// Returns true if the frame was rendered and is ready to be displayed.
bool Dx12Main::Render()
{
	// Don't try to render anything before the first Update.
	if (timer.GetFrameCount() == 0)
	{
		return false;
	}

	// Render the scene objects.
	// TODO: Replace this with your app's content rendering functions.
	return sceneRenderer->Render();
}

// Updates application state when the window's size changes (e.g. device orientation change)
void Dx12Main::OnWindowSizeChanged()
{
	// TODO: Replace this with the size-dependent initialization of your app's content.
	sceneRenderer->CreateWindowSizeDependentResources();
}

// Notifies the app that it is being suspended.
void Dx12Main::OnSuspending()
{
	// TODO: Replace this with your app's suspending logic.

	// Process lifetime management may terminate suspended apps at any time, so it is
	// good practice to save any state that will allow the app to restart where it left off.

	sceneRenderer->SaveState();

	// If your application uses video memory allocations that are easy to re-create,
	// consider releasing that memory to make it available to other applications.
}

// Notifes the app that it is no longer suspended.
void Dx12Main::OnResuming()
{
	// TODO: Replace this with your app's resuming logic.
}

// Notifies renderers that device resources need to be released.
void Dx12Main::OnDeviceRemoved()
{
	// TODO: Save any necessary application or renderer state and release the renderer
	// and its resources which are no longer valid.
	sceneRenderer->SaveState();
	sceneRenderer = nullptr;
}
