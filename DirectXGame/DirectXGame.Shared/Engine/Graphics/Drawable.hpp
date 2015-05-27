#pragma once

#include "Engine/Common/StepTimer.hpp"
#include "DeviceResources.hpp"

class Drawable
{
public:
	Drawable(const DeviceResourcesPtr& DeviceResources) : deviceResources(DeviceResources), isVisible(true) { }
	virtual ~Drawable() { }

	virtual void CreateDeviceResources() { } //Create any resources that depend on the graphics device
	virtual void CreateWindowResources(Windows::UI::Core::CoreWindow^ Window) { } //Create any resources that depend on the size of the window. Called after CreateDeviceResources()
	virtual void ReleaseDeviceResources() { } //Release any resources that depended on the graphics device
	virtual void Draw(const StepTimer& Timer) { } //Draw

	bool isVisible;

protected:
	DeviceResourcesPtr deviceResources;
};