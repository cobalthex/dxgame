#pragma once

#include "Engine/Common/Math.hpp"
#include "Engine/Graphics/DeviceResources.hpp"
#include "Engine/Graphics/ConstantBuffer.hpp"
#include "Engine/Graphics/Shaders/ShaderStructures.hpp"
#include "Engine/Graphics/Shaders/Shader.hpp"
#include "Engine/Graphics/Textures/TextureCache.hpp"
#include "Engine/Graphics/Shaders/LitShader.hpp"
#include "Engine/Graphics/Material.hpp"
#include "../DeviceResources.hpp"

namespace Osl { class Object; };

namespace Materials
{
	class UnlitMaterial : public Material
	{
	public:
		UnlitMaterial() { }
		UnlitMaterial(const std::shared_ptr<Shader>& Shader);
		UnlitMaterial(TextureCache& TexCache, const Osl::Object& MaterialObject, const std::shared_ptr<Shader>& Shader);

		std::shared_ptr<Texture2D> diffuseMap;
		std::shared_ptr<Shader> shader;
	};
};

namespace Shaders
{
	//A basic unlit shader that uses the same inputs as a lit shader
	class UnlitShader : public Shader
	{
	public:
		typedef LitShader::Vertex Vertex;

		UnlitShader() { }
		UnlitShader(const DeviceResourcesPtr& DeviceResources);

		inline void Apply() override
		{
			ActiveShader = this;

			object.BindVertex(0);

			vshader.Apply();
			pshader.Apply();
		}
		inline void Update() override
		{
			object.Update();
		}

		virtual inline ShaderType Type() const { return ShaderType::Unlit; }

		inline void SetInputLayout() const{ if (vshader.IsValid()) vshader.DeviceContext()->IASetInputLayout(inputLayout.Get()); }

		ConstantBuffer<ObjectBufferDef> object;

	protected:
		VertexShader vshader;
		PixelShader pshader;
		InputLayout inputLayout;
	};
}