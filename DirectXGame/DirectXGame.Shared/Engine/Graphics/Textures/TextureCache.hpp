#pragma once

#include "Engine/Graphics/DeviceResources.hpp"
#include "Engine/Data/Cache.hpp"
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

	virtual bool Reload() override; //reload all textures (where the key is the filename) onto the graphics device. (Returns false when one or more textures cannot be reloaded, are set to invalid)

protected:
	DeviceResourcesPtr deviceResources;
};