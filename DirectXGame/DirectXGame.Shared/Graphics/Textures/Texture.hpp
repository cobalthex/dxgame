#pragma once

#include "Pch.hpp"
#include "Graphics/DeviceResources.hpp"

//The basis for all textures, abstract
class Texture
{
protected:
	Texture() : deviceResources(nullptr), srv(nullptr) { }
	Texture(const DX::DeviceResourcesPtr& DeviceResources, const ComPtr<ID3D11ShaderResourceView>& ShaderResourceView)
		: deviceResources(DeviceResources), srv(ShaderResourceView) { }

	DX::DeviceResourcesPtr deviceResources;
	ComPtr<ID3D11ShaderResourceView> srv;
};