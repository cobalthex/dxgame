#pragma once

#include "Pch.hpp"
#include "Graphics/DeviceResources.hpp"
#include "Game/Game.hpp"

ref class App sealed : public Windows::ApplicationModel::Core::IFrameworkView
{
public:
	App();

	//IFrameworkView Methods.
	virtual void Initialize(Windows::ApplicationModel::Core::CoreApplicationView^ ApplicationView);
	virtual void SetWindow(Windows::UI::Core::CoreWindow^ Window);
	virtual void Load(Platform::String^ EntryPoint);
	virtual void Run();
	virtual void Uninitialize();

protected:
	//Application lifecycle event handlers.
	void OnActivated(Windows::ApplicationModel::Core::CoreApplicationView^ ApplicationView, Windows::ApplicationModel::Activation::IActivatedEventArgs^ Args);
	void OnSuspending(Platform::Object^ Sender, Windows::ApplicationModel::SuspendingEventArgs^ Args);
	void OnResuming(Platform::Object^ Sender, Platform::Object^ Args);

	//Window event handlers.
#if !(WINAPI_FAMILY == WINAPI_FAMILY_PHONE_APP)
	void OnWindowSizeChanged(Windows::UI::Core::CoreWindow^ Sender, Windows::UI::Core::WindowSizeChangedEventArgs^ Args);
#endif
	void OnVisibilityChanged(Windows::UI::Core::CoreWindow^ Sender, Windows::UI::Core::VisibilityChangedEventArgs^ Args);
	void OnWindowClosed(Windows::UI::Core::CoreWindow^ Sender, Windows::UI::Core::CoreWindowEventArgs^ Args);

	//DisplayInformation event handlers.
#if !(WINAPI_FAMILY == WINAPI_FAMILY_PHONE_APP)
	void OnDpiChanged(Windows::Graphics::Display::DisplayInformation^ Sender, Platform::Object^ Args);
	void OnOrientationChanged(Windows::Graphics::Display::DisplayInformation^ Sender, Platform::Object^ Args);
#endif
	void OnDisplayContentsInvalidated(Windows::Graphics::Display::DisplayInformation^ Sender, Platform::Object^ Args);

private:
	std::shared_ptr<DeviceResources> deviceResources;
	std::unique_ptr<Game> main;
	bool windowClosed;
	bool windowVisible;
};

ref class Direct3DApplicationSource sealed : Windows::ApplicationModel::Core::IFrameworkViewSource
{
public:
	virtual Windows::ApplicationModel::Core::IFrameworkView^ CreateView();
};
