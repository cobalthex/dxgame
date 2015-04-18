#include "Pch.hpp"
#include "ShadowMapEffect.hpp"

ShadowMapEffect::ShadowMapEffect(const DeviceResourcesPtr& DeviceResources, ShaderCache& ShCache)
	: Effect(DeviceResources), shader(std::static_pointer_cast<Shaders::ShadowMapShader>(ShCache.Load(ShaderType::ShadowMap)))
{

}

void ShadowMapEffect::Begin()
{
	//get depth texture
	auto dsv = deviceResources->GetDepthStencilView();

	shader->light.data.lightColor = light->color;
	shader->world.data.LightPosition = light->position;
	
	shader->world.data.lightWvp =  camera->Projection()
}
void ShadowMapEffect::End()
{

}