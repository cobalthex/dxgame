#pragma once

#include "Common/StepTimer.hpp"
#include "Graphics/DeviceResources.hpp"

//An abstract class that implements the basic functions of a rendered component. (This is mainly for low level)
class Renderer
{
public:
	Renderer(const DeviceResourcesPtr& DeviceResources) : deviceResources(DeviceResources) { }
	virtual ~Renderer() { }

	virtual void CreateDeviceResources() { } //Create any resources that depend on the graphics device
	virtual void CreateWindowResources(Windows::UI::Core::CoreWindow^ Window) { } //Create any resources that depend on the size of the window
	virtual void ReleaseDeviceResources() { } //Release any resources that depended on the graphics device
	virtual void Update(const StepTimer& Timer) { } //Update
	virtual void Render() = 0; //Render

protected:
	DeviceResourcesPtr deviceResources; //A pointer to the device resources that this renderer should use
};