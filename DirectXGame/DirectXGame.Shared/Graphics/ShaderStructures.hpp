#pragma once

#include "Pch.hpp"

namespace DirectXGame
{
	// Constant buffer used to send MVP matrices to the vertex shader.
	struct ModelViewProjectionConstantBuffer
	{
		DirectX::XMFLOAT4X4 model;
		DirectX::XMFLOAT4X4 view;
		DirectX::XMFLOAT4X4 projection;
	};


	// Used to send per-vertex data to the vertex shader.
	struct VertexPositionColor
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT3 color;

		static const D3D11_INPUT_ELEMENT_DESC ElementDesc[2];
	};

	//A single vertex that contains a position, normal, and a texture
	struct VertexPositionNormalTexture
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT2 texture;

		//The element description of this vertex
		static const D3D11_INPUT_ELEMENT_DESC ElementDesc[3];
	};
}