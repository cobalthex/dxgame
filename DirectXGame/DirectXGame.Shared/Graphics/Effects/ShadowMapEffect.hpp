#pragma once

#include "Effect.hpp"
#include "Graphics/Shaders/ShaderCache.hpp"
#include "Graphics/Shaders/ShadowMapShader.hpp"
#include "Graphics/Lighting.hpp"
#include "Graphics/Scene/Camera.hpp"

//Create a shadow map and apply it. This uses the built in depth stencil view to render.
class ShadowMapEffect : public Effect
{
public:
	ShadowMapEffect() : Effect(nullptr) { }
	ShadowMapEffect(const DeviceResourcesPtr& DeviceResources, ShaderCache& ShCache);

	virtual void Begin() override; //set up rendering for the shadow map
	virtual void End() override; //apply the shadow map

	Light* light; //the light to use as the shadow source
	Camera* camera; //the scene camera

protected:
	std::shared_ptr<Shaders::ShadowMapShader> shader;
	ComPtr<ID3D11DepthStencilView> shadowDepth;
	ComPtr<ID3D11Texture2D> shadowMap;
	ComPtr<ID3D11ShaderResourceView> shadowSRV;

	Camera lightCam;
};