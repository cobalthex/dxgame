#pragma once

#include "Graphics/DeviceResources.hpp"
#include "Data/Cache.hpp"
#include "Shader.hpp"

enum class ShaderType
{
	Unknown,
	PositionColor,
	PositionTexture,
	Lit,
	LitSkinned,

	ShadowMap
};

class ShaderCache : public Cache<ShaderType, Shader>
{
public:
	ShaderCache(const DeviceResourcesPtr& DeviceResources) : deviceResources(DeviceResources) { }
	virtual ~ShaderCache() { }

	virtual std::shared_ptr<Shader> Load(const ShaderType& Key) override;
	template <class ShType>
	inline std::shared_ptr<ShType> Load(const ShaderType& Key) { return std::static_pointer_cast<ShType>(Load(Key)); }

protected:
	DeviceResourcesPtr deviceResources;
};