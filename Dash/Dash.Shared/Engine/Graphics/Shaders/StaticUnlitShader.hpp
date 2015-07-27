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
	class StaticUnlitShader : public Shader
	{
	public:
		StaticUnlitShader() = default;
		StaticUnlitShader(const DeviceResourcesPtr& DeviceResources);

		inline void Apply() override
		{
			object.BindVertex(0);

			vshader.Apply();
			pshader.Apply();
		}
		inline void Update() override
		{
			object.Update();
		}

		virtual inline ShaderType Type() const { return ShaderType::StaticUnlit; }

		inline void SetInputLayout() const { if (vshader.IsValid()) vshader.DeviceContext()->IASetInputLayout(inputLayout.Get()); }

		ConstantBuffer<ObjectBufferDef> object;

	protected:
		VertexShader vshader;
		PixelShader pshader;
		InputLayout inputLayout;
	};
}