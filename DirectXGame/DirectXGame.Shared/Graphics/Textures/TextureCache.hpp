#pragma once

#include "Graphics/DeviceResources.hpp"
#include "Data/Cache.hpp"
#include "Texture2D.hpp"

class TextureCache : public Cache<std::string, Texture2D>
{
public:
	TextureCache(const DeviceResourcesPtr& DeviceResources) : deviceResources(DeviceResources) { }
	virtual ~TextureCache() { }

	virtual std::shared_ptr<Texture2D>& Load(const std::string& Key) override; //Load a texture, keys are stored as case-insensitive

protected:
	DeviceResourcesPtr deviceResources;
};