#pragma once

#include "Pch.hpp"
#include "Engine/Common/Math.hpp"
#include "Engine/Graphics/Textures/Texture2D.hpp"

using namespace Math;

enum class FillMode
{
	Solid = D3D11_FILL_SOLID,
	Wireframe = D3D11_FILL_WIREFRAME
};
enum class BlendMode
{
	Alpha,
	Additive
};

//The monolithic material for all standard models in game
//Shader usage determined on parameters and model type
class Material
{
public:
	Material() = default;

	FillMode fillMode;
	BlendMode blendMode;

	bool useLighting;
	Color emissive;
	Color diffuse;
	Color ambient;
	Color specular;
	float specularPower;

	bool useTexture;
	std::shared_ptr<Texture2D> texture;

	static Material FromFile(const std::string& File, TextureCache& TexCache);
};