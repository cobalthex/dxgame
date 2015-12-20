#pragma once

namespace Dx12
{
	// Constant buffer used to send MVP matrices to the vertex shader.
	struct ModelViewProjectionConstantBuffer
	{
		DirectX::XMFLOAT4X4 world;
		DirectX::XMFLOAT4X4 inverseTransposeWorld;
		DirectX::XMFLOAT4X4 worldViewProjection;
		DirectX::XMFLOAT4X4 joints[128];
	};
}