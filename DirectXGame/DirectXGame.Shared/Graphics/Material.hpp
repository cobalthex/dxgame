#pragma once

template <class ShaderType>
class Material
{
public:
	Material(const std::shared_ptr<ShaderType>& Shader) : shader(Shader) { }
	virtual ~Material() { }

	std::shared_ptr<ShaderType> shader; //the type of shader that this material uses
};