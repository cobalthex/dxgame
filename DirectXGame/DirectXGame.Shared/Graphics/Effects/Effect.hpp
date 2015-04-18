#pragma once

#include "Graphics/DeviceResources.hpp"

//The base for an effect
//Effects are usually post-processes like shadows
class Effect
{
public:
	virtual void Begin() = 0;
	virtual void End() = 0;

protected:
	Effect(const DeviceResourcesPtr& DeviceResources) : deviceResources(DeviceResources) { }
	DeviceResourcesPtr deviceResources;
};