#pragma once

#include "Engine/Common/Math.hpp"
#include "Engine/Graphics/DeviceResources.hpp"
#include "Engine/Graphics/ConstantBuffer.hpp"
#include "Engine/Graphics/Shaders/ShaderStructures.hpp"
#include "Engine/Graphics/Shaders/Shader.hpp"
#include "Engine/Graphics/Lighting.hpp"
#include "Engine/Graphics/Textures/TextureCache.hpp"
#include "Engine/Graphics/Material.hpp"

namespace Shaders
{
	class StaticLitShader : public Shader
	{
	public:
		struct MaterialBufferDef : public ConstantBufferDef
		{
			Color emissive;
			Color ambient;
			Color diffuse;
			Color specular;
			float specularPower;
			bool useTexture;
		};

		StaticLitShader() = default;
		StaticLitShader(const DeviceResourcesPtr& DeviceResources);

		inline void Apply() override
		{
			object.BindVertex(0);
			material.BindPixel(0);
			lighting.BindPixel(1);

			vshader.Apply();
			pshader.Apply();
		}
		inline void Update() override
		{
			object.Update();
			material.Update();
			lighting.Update();
		}

		virtual void ApplyMaterial(const Material& Material);
		virtual inline ShaderType Type() const { return ShaderType::StaticLit; }

		inline void SetInputLayout() const { if (vshader.IsValid()) vshader.DeviceContext()->IASetInputLayout(inputLayout.Get()); }

		ConstantBuffer<ObjectBufferDef> object;
		ConstantBuffer<MaterialBufferDef> material;
		ConstantBuffer<LightBufferDef> lighting;

	protected:
		VertexShader vshader;
		PixelShader pshader;
		InputLayout inputLayout;
	};
}