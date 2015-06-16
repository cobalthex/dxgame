#pragma once

#include "Pch.hpp"
#include "Engine/Graphics/DeviceResources.hpp"

//The basis for all textures, abstract
class Texture
{
public:
	inline ComPtr<ID3D11ShaderResourceView> GetShaderResourceView() const { return srv; }

protected:
	Texture() : deviceResources(nullptr), srv(nullptr) { }
	Texture(const DeviceResourcesPtr& DeviceResources, const ComPtr<ID3D11ShaderResourceView>& ShaderResourceView)
		: deviceResources(DeviceResources), srv(ShaderResourceView) { }

	DeviceResourcesPtr deviceResources;
	ComPtr<ID3D11ShaderResourceView> srv;
};