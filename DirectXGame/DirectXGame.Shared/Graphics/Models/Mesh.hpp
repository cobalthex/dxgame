#pragma once

#include "Pch.hpp"
#include "Common/DeviceResources.hpp"

//A single mesh that contains a vertex and index buffer as well as material
class Mesh
{
public:
	typedef ComPtr<ID3D11Buffer> VertexBuffer;
	typedef ComPtr<ID3D11Buffer> IndexBuffer;

	const VertexBuffer& Vertices() const { return indices; }
	const IndexBuffer& Indices() const { return indices; }

protected:
	VertexBuffer vertices;
	IndexBuffer indices;
	size_t vertexCount;
	size_t indexCount;

	static const unsigned VertexStride;
};