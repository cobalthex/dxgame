#pragma once

#include "Pch.hpp"
#include "Graphics/Textures/Texture2D.hpp"
#include "Graphics/ConstantBuffer.hpp"
#include "Common/SimpleMath.hpp"

//A material for a single mesh of a model
class Material
{
public:
	Texture2D* texture; //The color map for the material
	bool useTexture = true;

	DirectX::SimpleMath::Color emissive;
	DirectX::SimpleMath::Color ambient;
	DirectX::SimpleMath::Color diffuse;
	DirectX::SimpleMath::Color specular;
	float specularPower;
};

struct MaterialConstantBufferDef : public ConstantBufferDef
{
	DirectX::XMVECTOR emissive;
	DirectX::XMVECTOR ambient;
	DirectX::XMVECTOR diffuse;
	DirectX::XMVECTOR specular;
	float specularPower;
	bool useTexture;

	void FillFromMaterial(const Material& Material)
	{
		ambient = Material.ambient;
		diffuse = Material.diffuse;
		specular = Material.specular;
		specularPower = Material.specularPower;
		useTexture = Material.useTexture;
	}
};