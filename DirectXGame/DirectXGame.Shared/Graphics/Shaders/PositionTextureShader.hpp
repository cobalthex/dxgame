#pragma once

#include "Common/Math.hpp"
#include "Graphics/DeviceResources.hpp"
#include "Graphics/ConstantBuffer.hpp"
#include "Graphics/Shaders/ShaderStructures.hpp"
#include "Graphics/Shaders/Shader.hpp"

namespace Shaders
{
	class PositionTextureShader : public Shader
	{
	public:
		struct Vertex
		{
			DirectX::Math::Vector3 position;
			DirectX::Math::Vector2 texCoord;

			Vertex(const DirectX::Math::Vector3& Position, const DirectX::Math::Vector2& TexCoord) : position(Position), texCoord(TexCoord) { }

			static const D3D11_INPUT_ELEMENT_DESC ElementDesc[];
			static const unsigned ElementCount;
		};

		PositionTextureShader() { }
		PositionTextureShader(const DeviceResourcesPtr& DeviceResources);

		inline void Apply() override
		{
			ActiveShader = this;

			wvp.BindVertex(0);

			vshader.Apply();
			pshader.Apply();
		}
		inline void Update() override
		{
			wvp.Update();
		}

		virtual inline ShaderType Type() const { return ShaderType::PositionTexture; }

		inline void SetInputLayout() const{ if (vshader.IsValid()) vshader.DeviceContext()->IASetInputLayout(inputLayout.Get()); }

		ConstantBuffer<WVPBufferDef> wvp; //world view projection buffer

	protected:
		VertexShader vshader;
		PixelShader pshader;
		InputLayout inputLayout;
	};
}