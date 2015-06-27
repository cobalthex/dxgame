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

	lightCam = *camera;
	lightCam.position = light->position.XYZ();
	lightCam.lookAt = light->direction.XYZ();
	lightCam.CalcMatrices();
	
	shader->world.data.world = Matrix::CreateWorld(lightCam.position, light->direction.XYZ(), Vector3::Up);
	shader->world.data.LightPosition = light->position;
	lightCam.CalcMatrices();
	//shader->world.data.lightWvp = 
}
void ShadowMapEffect::End()
{

}