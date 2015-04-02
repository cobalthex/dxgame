#pragma once

#include "Common/SimpleMath.hpp"
#include "Graphics/DeviceResources.hpp"
#include "Graphics/ConstantBuffer.hpp"
#include "Graphics/Shaders/ShaderStructures.hpp"
#include "Graphics/Textures/TextureCache.hpp"
#include "Graphics/Shaders/Shader.hpp"
#include "Graphics/Material.hpp"
#include "Graphics/Lighting.hpp"

namespace Osl { class Object; };
namespace Shaders { class LitSkinnedShader; }

namespace Materials
{
	class LitSkinnedMaterial : public Material<Shaders::LitSkinnedShader>
	{
	public:
		LitSkinnedMaterial() : Material(nullptr) { }
		LitSkinnedMaterial(TextureCache& TexCache, const Osl::Object& MaterialObject, const std::shared_ptr<Shaders::LitSkinnedShader>& Shader = nullptr);

		Color emissive;
		Color ambient;
		Color diffuse;
		Color specular;
		float specularPower;

		bool useTexture;
		std::shared_ptr<Texture2D> diffuseMap;
	};
};

namespace Shaders
{
	class LitSkinnedShader : public Shader
	{
	public:

		struct Vertex
		{
			Vector3 position;
			Vector3 normal;
			Vector4 tangent;
			Vector2 texCoord;
			DirectX::PackedVector::XMUBYTEN4 color;
			DirectX::PackedVector::XMUBYTE4 indices;
			DirectX::PackedVector::XMUBYTEN4 weights;

			//The element description of this vertex
			static const D3D11_INPUT_ELEMENT_DESC ElementDesc[];
			static const unsigned ElementCount;
		};

		//The constant buffer definition for the material that this shader uses
		struct MaterialBufferDef : public ConstantBufferDef
		{
			Color emissive;
			Color ambient;
			Color diffuse;
			Color specular;
			float specularPower;
			bool UseTexture;

			inline void operator = (const Materials::LitSkinnedMaterial& Material)
			{
				emissive = Material.emissive;
				ambient = Material.ambient;
				diffuse = Material.diffuse;
				specular = Material.specular;
				specularPower = Material.specularPower;
				UseTexture = Material.useTexture;
			}
		};

		LitSkinnedShader() { }
		LitSkinnedShader(const DeviceResourcesPtr& DeviceResources);

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
		ConstantBuffer<MaterialBufferDef> material;
		ConstantBuffer<LightBufferDef> lighting;

	protected:
		VertexShader vshader;
		PixelShader pshader;
		InputLayout inputLayout;
	};
}