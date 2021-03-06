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

class TextureCache;
enum class ShaderType;
enum class VertexFormats;

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

	ShaderType GetShaderType(VertexFormats VertexFormat) const; //Specify the correct shader that this material uses based on this material's properties and the vertex format specified

	static std::map<std::string, Material> LoadAllFromFile(const std::string& File, TextureCache& TexCache); //Load all of the materials from a file, stored by name
};