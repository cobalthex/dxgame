#include "Pch.hpp"
#include "ShadowMapEffect.hpp"

ShadowMapEffect::ShadowMapEffect(const DeviceResourcesPtr& DeviceResources, ShaderCache& ShCache)
	: Effect(DeviceResources), shader(std::static_pointer_cast<Shaders::ShadowMapShader>(ShCache.Load(ShaderType::ShadowMap)))
{

}

void ShadowMapEffect::Begin()
{

}
void ShadowMapEffect::End()
{

}