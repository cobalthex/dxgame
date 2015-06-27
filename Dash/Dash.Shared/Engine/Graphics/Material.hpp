#pragma once

//The parent for all materials, either for physics of shaders
class Material
{
public:
	Material(const std::shared_ptr<Shader>& Shader = nullptr)
		: shader(Shader) { }

	std::shared_ptr<Shader> shader;

	virtual void Apply() const = 0; //apply this material to a shader
};