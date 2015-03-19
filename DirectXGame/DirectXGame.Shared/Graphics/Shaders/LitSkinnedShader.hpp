#pragma once

#include "Common/SimpleMath.hpp"
#include "Graphics/DeviceResources.hpp"
#include "Graphics/ConstantBuffer.hpp"
#include "Graphics/Shaders/ShaderStructures.hpp"
#include "Graphics/Shaders/Shader.hpp"
#include "Graphics/Models/Material.hpp"
#include "Graphics/Lighting.hpp"

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

		LitSkinnedShader() : vshader(), pshader(), inputLayout(nullptr), object(), material(), lighting() { }
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

		ConstantBuffer<ObjectConstantBufferDef> object;
		ConstantBuffer<MaterialConstantBufferDef> material;
		ConstantBuffer<LightConstantBufferDef> lighting;

	protected:
		VertexShader vshader;
		PixelShader pshader;
		InputLayout inputLayout;
	};
}