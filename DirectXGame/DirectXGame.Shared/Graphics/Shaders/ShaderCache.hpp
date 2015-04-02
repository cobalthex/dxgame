#pragma once

#include "Graphics/DeviceResources.hpp"
#include "Data/Cache.hpp"
#include "Shader.hpp"

enum class ShaderType
{
	Unknown,
	PositionColor,
	LitSkinned
};

class ShaderCache : public Cache<ShaderType, Shader>
{
public:
	ShaderCache(const DeviceResourcesPtr& DeviceResources) : deviceResources(DeviceResources) { }
	virtual ~ShaderCache() { }

	virtual std::shared_ptr<Shader> Load(const ShaderType& Key) override;

protected:
	DeviceResourcesPtr deviceResources;
};