#pragma once

#include "Pch.hpp"
#include "Graphics/Textures/Texture2D.hpp"

//A material for a single mesh of a model
class Material
{
public:
	Texture2D* texture; //The color map for the material

	DirectX::XMVECTOR ambient;
	DirectX::XMVECTOR diffuse;
	DirectX::XMVECTOR specular;
	float specularPower;
};