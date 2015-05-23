#pragma once

#include "Engine/Graphics/DeviceResources.hpp"
#include "Engine/Data/Cache.hpp"
#include "Shader.hpp"

class ShaderCache : public Cache<ShaderType, Shader>
{
public:
	ShaderCache(const DeviceResourcesPtr& DeviceResources) : deviceResources(DeviceResources) { }
	virtual ~ShaderCache() { }

	virtual std::shared_ptr<Shader> Load(const ShaderType& Key) override;
	template <class ShType>
	inline std::shared_ptr<ShType> Load(const ShaderType& Key) { return std::static_pointer_cast<ShType>(Load(Key)); }

	virtual bool Reload() override; //Reload all of the shaders in the cache

protected:
	DeviceResourcesPtr deviceResources;
};