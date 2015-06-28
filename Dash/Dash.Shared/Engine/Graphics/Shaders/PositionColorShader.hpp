#pragma once

#include "Engine/Common/Math.hpp"
#include "Engine/Graphics/DeviceResources.hpp"
#include "Engine/Graphics/ConstantBuffer.hpp"
#include "Engine/Graphics/Shaders/ShaderStructures.hpp"
#include "Engine/Graphics/Shaders/Shader.hpp"

namespace Shaders
{
	class PositionColorShader : public Shader
	{
	public:
		struct Vertex
		{
			Math::Vector3 position;
			Math::Color color;

			static const D3D11_INPUT_ELEMENT_DESC ElementDesc[];
			static const unsigned ElementCount;
		};

		PositionColorShader() { }
		PositionColorShader(const DeviceResourcesPtr& DeviceResources);

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

		virtual inline ShaderType Type() const override { return ShaderType::PositionColor; }

		virtual inline void SetInputLayout() const override { if (vshader.IsValid()) vshader.DeviceContext()->IASetInputLayout(inputLayout.Get()); }
		
		ConstantBuffer<WVPBufferDef> wvp; //world view projection buffer

	protected:
		VertexShader vshader;
		PixelShader pshader;
		InputLayout inputLayout;
	};
}