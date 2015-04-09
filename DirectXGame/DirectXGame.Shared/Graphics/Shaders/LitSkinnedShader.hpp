#pragma once

#include "LitShader.hpp"

struct SkinnedObjectBufferDef : public ObjectBufferDef
{
	Matrix joints[MAX_JOINTS];
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

		ConstantBuffer<SkinnedObjectBufferDef> object;
		ConstantBuffer<Materials::LitMaterial::BufferDef> material;
		ConstantBuffer<LightBufferDef> lighting;

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

		inline void SetInputLayout() const{ if (vshader.IsValid()) vshader.DeviceContext()->IASetInputLayout(inputLayout.Get()); }

	protected:
		VertexShader vshader;
		PixelShader pshader;
		InputLayout inputLayout;
	};
}