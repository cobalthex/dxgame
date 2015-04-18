#pragma once

#include "Common/Math.hpp"
#include "Graphics/DeviceResources.hpp"
#include "Graphics/ConstantBuffer.hpp"
#include "Graphics/Shaders/ShaderStructures.hpp"
#include "Graphics/Shaders/Shader.hpp"
#include "Graphics/Lighting.hpp"
#include "Graphics/Textures/TextureCache.hpp"
#include "Graphics/Material.hpp"

namespace Osl { class Object; };

namespace Materials
{
	class LitMaterial : public Material
	{
	public:
		LitMaterial() { }
		LitMaterial(const std::shared_ptr<Shader>& Shader);
		LitMaterial(TextureCache& TexCache, const Osl::Object& MaterialObject, const std::shared_ptr<Shader>& Shader);

		Color emissive;
		Color ambient;
		Color diffuse;
		Color specular;
		float specularPower;

		bool useTexture;
		std::shared_ptr<Texture2D> diffuseMap;

		std::shared_ptr<Shader> shader;

		//The constant buffer definition for the material that this shader uses
		struct BufferDef : public ConstantBufferDef
		{
			Color emissive;
			Color ambient;
			Color diffuse;
			Color specular;
			float specularPower;
			bool UseTexture;

			inline void operator = (const Materials::LitMaterial& Material)
			{
				emissive = Material.emissive;
				ambient = Material.ambient;
				diffuse = Material.diffuse;
				specular = Material.specular;
				specularPower = Material.specularPower;
				UseTexture = Material.useTexture;
			}
		};
	};
};

namespace Shaders
{
	class LitShader : public Shader
	{
	public:

		struct Vertex
		{
			Vector3 position;
			Vector3 normal;
			Vector4 tangent;
			Vector2 texCoord;
			DirectX::PackedVector::XMUBYTEN4 color;

			//The element description of this vertex
			static const D3D11_INPUT_ELEMENT_DESC ElementDesc[];
			static const unsigned ElementCount;
		};

		LitShader() { }
		LitShader(const DeviceResourcesPtr& DeviceResources);

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

		inline void SetInputLayout() const{ vshader.DeviceContext()->IASetInputLayout(inputLayout.Get()); }

		ConstantBuffer<ObjectBufferDef> object;
		ConstantBuffer<Materials::LitMaterial::BufferDef> material;
		ConstantBuffer<LightBufferDef> lighting;

	protected:
		VertexShader vshader;
		PixelShader pshader;
		InputLayout inputLayout;
	};
}