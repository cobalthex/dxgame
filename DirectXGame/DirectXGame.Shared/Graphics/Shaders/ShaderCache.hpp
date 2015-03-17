#pragma once

#include "Graphics/DeviceResources.hpp"
#include "Common/Cache.hpp"
#include "Shader.hpp"

class ShaderCache : public Cache<std::string, Shader>
{
public:
	ShaderCache(const DX::DeviceResourcesPtr& DeviceResources) { }
	virtual ~ShaderCache() { }

	virtual std::shared_ptr<Shader>& Load(const std::string& Key) override;

protected:
	DX::DeviceResourcesPtr deviceResources;
};