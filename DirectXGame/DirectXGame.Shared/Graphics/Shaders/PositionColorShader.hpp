#pragma once

#include "Common/Math.hpp"
#include "Graphics/DeviceResources.hpp"
#include "Graphics/ConstantBuffer.hpp"
#include "Graphics/Shaders/ShaderStructures.hpp"
#include "Graphics/Shaders/Shader.hpp"

namespace Shaders
{
	class PositionColorShader : public Shader
	{
	public:
		struct Vertex
		{
			DirectX::Math::Vector3 position;
			DirectX::Math::Color color;

			static const D3D11_INPUT_ELEMENT_DESC ElementDesc[];
			static const unsigned ElementCount;
		};

		PositionColorShader() { }
		PositionColorShader(const DeviceResourcesPtr& DeviceResources);

		inline void Apply() override
		{
			wvp.BindVertex(0);

			vshader.Apply();
			pshader.Apply();
		}
		inline void Update() override
		{
			wvp.Update();
		}

		inline void SetInputLayout() const{ if (vshader.IsValid()) vshader.DeviceContext()->IASetInputLayout(inputLayout.Get()); }
		
		ConstantBuffer<WVPBufferDef> wvp; //world view projection buffer

	protected:
		VertexShader vshader;
		PixelShader pshader;
		InputLayout inputLayout;
	};
}