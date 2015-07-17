#pragma once

#include "Pch.hpp"
#include "Engine/Data/Cache.hpp"
#include "Model.hpp"

class ShaderCache;
class TextureCache;

class ModelCache : public Cache<std::string, Model>
{
public:
	ModelCache(const DeviceResourcesPtr& DeviceResources, TextureCache* TexCache, ShaderCache* ShCache) : deviceResources(DeviceResources), texCache(TexCache), shCache(ShCache) { }
	virtual ~ModelCache() { }

	//Load a texture, keys are stored as case-insensitive.
	//If Key is prefixed with / signify that the path is absolute
	//If Key is prefixed with ~ signify that the path is already rooted (~ is removed)
	//All others use standard folder
	virtual std::shared_ptr<Model> Load(const std::string& Key) override;

	virtual bool Reload() override; //reload all textures (where the key is the filename) onto the graphics device. (Returns false when one or more textures cannot be reloaded, are set to invalid)

protected:
	DeviceResourcesPtr deviceResources;
	TextureCache* texCache;
	ShaderCache* shCache;
};