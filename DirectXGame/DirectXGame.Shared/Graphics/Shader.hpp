#pragma once

#include "Pch.hpp"
#include "Common/Helpers.hpp"
#include "Common/IContent.hpp"
#include "Graphics/DeviceResources.hpp"

//Available shader types
enum class ShaderType
{
	Unknown,
	Compute,
	Vertex,
	Hull,
	Domain,
	Geometry,
	Pixel
};

//A single shader. Can be of any type
class Shader : public IContent
{
public:
	Shader() : devContext(nullptr), type(ShaderType::Unknown), shader(nullptr) { }
	virtual ~Shader() { }

	inline ShaderType Type() const { return type; }

	void Apply() const; //Apply this shader

protected:
	ComPtr<ID3D11DeviceContext> devContext;

	ShaderType type;
	ComPtr<ID3D11DeviceChild> shader;
};