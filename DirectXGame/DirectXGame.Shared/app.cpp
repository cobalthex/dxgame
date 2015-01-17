#include "Pch.hpp"
#include "App.hpp"

#include <ppltasks.h>

using namespace DirectXGame;

using namespace concurrency;
using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::UI::Core;
using namespace Windows::UI::Input;
using namespace Windows::System;
using namespace Windows::Foundation;
using namespace Windows::Graphics::Display;

// The main function is only used to initialize our IFrameworkView class.
[Platform::MTAThread]
int main(Platform::Array<Platform::String^>^)
{
	auto direct3DApplicationSource = ref new Direct3DApplicationSource();
	CoreApplication::Run(direct3DApplicationSource);
	return 0;
}

IFrameworkView^ Direct3DApplicationSource::CreateView()
{
	return ref new App();
}

App::App() :
	windowClosed(false),
	windowVisible(true)
{
}

// The first method called when the IFrameworkView is being created.
void App::Initialize(CoreApplicationView^ ApplicationView)
{
	// Register event handlers for app lifecycle. This example includes Activated, so that we
	// can make the CoreWindow active and start rendering on the window.
	ApplicationView->Activated +=
		ref new TypedEventHandler<CoreApplicationView^, IActivatedEventArgs^>(this, &App::OnActivated);

	CoreApplication::Suspending +=
		ref new EventHandler<SuspendingEventArgs^>(this, &App::OnSuspending);

	CoreApplication::Resuming +=
		ref new EventHandler<Platform::Object^>(this, &App::OnResuming);

	// At this point we have access to the device. 
	// We can create the device-dependent resources.
	deviceResources = std::make_shared<DX::DeviceResources>();
}

// Called when the CoreWindow object is created (or re-created).
void App::SetWindow(CoreWindow^ Window)
{
	Window->VisibilityChanged +=
		ref new TypedEventHandler<CoreWindow^, VisibilityChangedEventArgs^>(this, &App::OnVisibilityChanged);

	Window->Closed += 
		ref new TypedEventHandler<CoreWindow^, CoreWindowEventArgs^>(this, &App::OnWindowClosed);

	DisplayInformation^ currentDisplayInformation = DisplayInformation::GetForCurrentView();

	DisplayInformation::DisplayContentsInvalidated +=
		ref new TypedEventHandler<DisplayInformation^, Object^>(this, &App::OnDisplayContentsInvalidated);

#if !(WINAPI_FAMILY == WINAPI_FAMILY_PHONE_APP)
	Window->SizeChanged +=
		ref new TypedEventHandler<CoreWindow^, WindowSizeChangedEventArgs^>(this, &App::OnWindowSizeChanged);

	currentDisplayInformation->DpiChanged +=
		ref new TypedEventHandler<DisplayInformation^, Object^>(this, &App::OnDpiChanged);

	currentDisplayInformation->OrientationChanged +=
		ref new TypedEventHandler<DisplayInformation^, Object^>(this, &App::OnOrientationChanged);

	// Disable all pointer visual feedback for better performance when touching.
	// This is not supported on Windows Phone applications.
	auto pointerVisualizationSettings = PointerVisualizationSettings::GetForCurrentView();
	pointerVisualizationSettings->IsContactFeedbackEnabled = false; 
	pointerVisualizationSettings->IsBarrelButtonFeedbackEnabled = false;
#endif

	deviceResources->SetWindow(Window);
}

// Initializes scene resources, or loads a previously saved app state.
void App::Load(Platform::String^ EntryPoint)
{
	if (main == nullptr)
		main = std::unique_ptr<DirectXGameMain>(new DirectXGameMain(deviceResources));
}

// This method is called after the window becomes active.
void App::Run()
{
	while (!windowClosed)
	{
		if (windowVisible)
		{
			CoreWindow::GetForCurrentThread()->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessAllIfPresent);

			main->Update();

			if (main->Render())
				deviceResources->Present();
		}
		else
			CoreWindow::GetForCurrentThread()->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessOneAndAllPending);
	}
}

// Required for IFrameworkView.
// Terminate events do not cause Uninitialize to be called. It will be called if your IFrameworkView
// class is torn down while the app is in the foreground.
void App::Uninitialize()
{
}

// Application lifecycle event handlers.

void App::OnActivated(CoreApplicationView^ ApplicationView, IActivatedEventArgs^ Args)
{
	// Run() won't start until the CoreWindow is activated.
	CoreWindow::GetForCurrentThread()->Activate();
}

void App::OnSuspending(Platform::Object^ Sender, SuspendingEventArgs^ Args)
{
	// Save app state asynchronously after requesting a deferral. Holding a deferral
	// indicates that the application is busy performing suspending operations. Be
	// aware that a deferral may not be held indefinitely. After about five seconds,
	// the app will be forced to exit.
	SuspendingDeferral^ deferral = Args->SuspendingOperation->GetDeferral();

	create_task([this, deferral]()
	{
        deviceResources->Trim();

		// Insert your code here.

		deferral->Complete();
	});
}

void App::OnResuming(Platform::Object^ Sender, Platform::Object^ Args)
{
	// Restore any data or state that was unloaded on suspend. By default, data
	// and state are persisted when resuming from suspend. Note that this event
	// does not occur if the app was previously terminated.

	// Insert your code here.
}

// Window event handlers.

void App::OnVisibilityChanged(CoreWindow^ Sender, VisibilityChangedEventArgs^ Args)
{
	windowVisible = Args->Visible;
}

void App::OnWindowClosed(CoreWindow^ Sender, CoreWindowEventArgs^ Args)
{
	windowClosed = true;
}

#if !(WINAPI_FAMILY == WINAPI_FAMILY_PHONE_APP)
void App::OnWindowSizeChanged(CoreWindow^ Sender, WindowSizeChangedEventArgs^ Args)
{
	deviceResources->SetLogicalSize(Size(Sender->Bounds.Width, Sender->Bounds.Height));
	main->CreateWindowSizeDependentResources();
}
#endif

// DisplayInformation event handlers.

void App::OnDisplayContentsInvalidated(DisplayInformation^ Sender, Object^ Args)
{
	deviceResources->ValidateDevice();
}


#if !(WINAPI_FAMILY == WINAPI_FAMILY_PHONE_APP)
void App::OnDpiChanged(DisplayInformation^ Sender, Object^ Args)
{
	deviceResources->SetDpi(Sender->LogicalDpi);
	main->CreateWindowSizeDependentResources();
}

void App::OnOrientationChanged(DisplayInformation^ Sender, Object^ Args)
{
	deviceResources->SetCurrentOrientation(Sender->CurrentOrientation);
	main->CreateWindowSizeDependentResources();
}
#endif