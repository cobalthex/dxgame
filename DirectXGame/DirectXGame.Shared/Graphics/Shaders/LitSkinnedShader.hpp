#pragma once

#include "LitShader.hpp"

namespace Shaders
{
	class LitSkinnedShader : public LitShader
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

		LitSkinnedShader(const DeviceResourcesPtr& DeviceResources);
	};
}