#pragma once

#include "Graphics/DeviceResources.hpp"
#include "Data/Cache.hpp"
#include "Texture2D.hpp"

class TextureCache : public Cache<std::string, Texture2D>
{
public:
	TextureCache(const DeviceResourcesPtr& DeviceResources) : deviceResources(DeviceResources) { }
	virtual ~TextureCache() { }

	//Load a texture, keys are stored as case-insensitive.
	//If Key is prefixed with / signify that the path is absolute
	//If Key is prefixed with ~ signify that the path is already rooted (~ is removed)
	//All others use standard folder
	virtual std::shared_ptr<Texture2D> Load(const std::string& Key) override;

protected:
	DeviceResourcesPtr deviceResources;
};