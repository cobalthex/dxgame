#pragma once

#include "Pch.hpp"
#include "Graphics/DeviceResources.hpp"
#include "Common/IContent.hpp"

//The basis for all textures, abstract
class Texture : public IContent
{
protected:
	Texture() : deviceResources(nullptr), srv(nullptr) { }
	Texture(const DX::DeviceResourcesPtr& DeviceResources, const ComPtr<ID3D11ShaderResourceView>& ShaderResourceView)
		: deviceResources(DeviceResources), srv(ShaderResourceView) { }

	DX::DeviceResourcesPtr deviceResources;
	ComPtr<ID3D11ShaderResourceView> srv;
};