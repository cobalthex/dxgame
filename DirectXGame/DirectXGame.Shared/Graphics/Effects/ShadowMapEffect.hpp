#pragma once

#include "Effect.hpp"
#include "Graphics/Shaders/ShaderCache.hpp"
#include "Graphics/Shaders/ShadowMapShader.hpp"

class ShadowMapEffect : public Effect
{
public:
	ShadowMapEffect() : Effect(nullptr) { }
	ShadowMapEffect(const DeviceResourcesPtr& DeviceResources, ShaderCache& ShCache);

	virtual void Begin() override; //set up rendering for the shadow map
	virtual void End() override; //apply the shadow map

protected:
	std::shared_ptr<Shaders::ShadowMapShader> shader;
	ComPtr<ID3D11DepthStencilView> shadowDepth;
	ComPtr<ID3D11Texture2D> shadowMap;
	ComPtr<ID3D11ShaderResourceView> shadowSRV;
};