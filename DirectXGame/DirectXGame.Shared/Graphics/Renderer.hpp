#pragma once

#include "Common/StepTimer.hpp"
#include "Common/DeviceResources.hpp"

//An abstract class that implements the basic functions of a rendered component
class Renderer
{
public:
	Renderer(const DX::DeviceResourcesPtr& DeviceResources) : deviceResources(DeviceResources) { }
	virtual ~Renderer() { }

	virtual void CreateDeviceDependentResources() { } //Create any resources that depend on the graphics device
	virtual void CreateWindowSizeDependentResources() { } //Create any resources that depend on the size of the window
	virtual void ReleaseDeviceDependentResources() { } //Release any resources that depended on the graphics device
	virtual void Update(const DX::StepTimer& Timer) { } //Update
	virtual void Render() = 0; //Render

protected:
	DX::DeviceResourcesPtr deviceResources; //A pointer to the device resources that this renderer should use
};