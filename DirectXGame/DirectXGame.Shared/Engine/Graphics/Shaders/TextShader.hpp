#pragma once

#include "Engine/Common/Math.hpp"
#include "Engine/Graphics/DeviceResources.hpp"
#include "Engine/Graphics/ConstantBuffer.hpp"
#include "Engine/Graphics/Shaders/ShaderStructures.hpp"
#include "Engine/Graphics/Shaders/Shader.hpp"
#include "Engine/Graphics/Shaders/PositionTextureShader.hpp"

struct ColorBufferDef : public ConstantBufferDef
{
	Color color;
};

namespace Shaders
{
	class TextShader : public Shader
	{
	public:
		typedef PositionTextureShader::Vertex Vertex;

		TextShader() { }
		explicit TextShader(const DeviceResourcesPtr& DeviceResources);

		inline void Apply() override
		{
			ActiveShader = this;

			wvp.BindVertex(0);
			color.BindPixel(0);

			vshader.Apply();
			pshader.Apply();
		}
		inline void Update() override
		{
			wvp.Update();
			color.Update();
		}

		virtual inline ShaderType Type() const { return ShaderType::Text; }

		inline void SetInputLayout() const{ if (vshader.IsValid()) vshader.DeviceContext()->IASetInputLayout(inputLayout.Get()); }

		ConstantBuffer<WVPBufferDef> wvp; //world view projection buffer
		ConstantBuffer<ColorBufferDef> color; //the color of the text

	protected:
		VertexShader vshader;
		PixelShader pshader;
		InputLayout inputLayout;
	};
}