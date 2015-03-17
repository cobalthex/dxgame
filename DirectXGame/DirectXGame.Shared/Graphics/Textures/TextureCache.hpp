#pragma once

#include "Graphics/DeviceResources.hpp"
#include "Common/Cache.hpp"
#include "Texture2D.hpp"

class TextureCache : public Cache<std::string, Texture2D>
{
public:
	TextureCache(const DX::DeviceResourcesPtr& DeviceResources) { }
	virtual ~TextureCache() { }

	virtual std::shared_ptr<Texture2D>& Load(const std::string& Key) override;

protected:
	DX::DeviceResourcesPtr deviceResources;
};