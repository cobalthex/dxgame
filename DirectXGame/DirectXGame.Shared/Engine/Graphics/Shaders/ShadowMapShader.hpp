#pragma once
#pragma once

#include "Shader.hpp"
#include "Engine/Common/Math.hpp"
#include "Engine/Graphics/ConstantBuffer.hpp"
#include "ShaderStructures.hpp"

using namespace Math;

struct ShadowBufferDef : public ConstantBufferDef
{
	Matrix world;
	Matrix worldViewProjection;
	Matrix lightWvp;
	Vector4 LightPosition; //W unused
};
struct LightColorBufferDef : public ConstantBufferDef
{
	Color lightColor;
};

namespace Shaders
{
	class ShadowMapShader : public Shader
	{
	public:
		struct Vertex
		{
			Vector3 position;
			Vector3 normal;
			Vector2 texCoord;

			//The element description of this vertex
			static const D3D11_INPUT_ELEMENT_DESC ElementDesc[];
			static const unsigned ElementCount;
		};

		ConstantBuffer<ShadowBufferDef> world;
		ConstantBuffer<LightColorBufferDef> light;

		ShadowMapShader() { }
		ShadowMapShader(const DeviceResourcesPtr& DeviceResources);

		inline void Apply() override
		{
			ActiveShader = this;

			world.BindVertex(0);
			light.BindPixel(0);

			vshader.Apply();
			pshader.Apply();
		}
		inline void Update() override
		{
			world.Update();
			light.Update();
		}

		virtual inline ShaderType Type() const { return ShaderType::ShadowMap; }

		inline void SetInputLayout() const{ if (vshader.IsValid()) vshader.DeviceContext()->IASetInputLayout(inputLayout.Get()); }

	protected:
		VertexShader vshader;
		PixelShader pshader;
		InputLayout inputLayout;
	};
}