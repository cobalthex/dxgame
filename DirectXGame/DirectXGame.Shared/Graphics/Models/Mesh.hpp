#pragma once

#include "Pch.hpp"
#include "Common/DeviceResources.hpp"
#include "Graphics/ShaderStructures.hpp"

//A single mesh that contains a vertex and index buffer as well as material
class Mesh
{
public:
	Mesh() : vertices(nullptr), indices(nullptr), vertexCount(0), indexCount(0), deviceResources(nullptr) { }
	Mesh(const DX::DeviceResourcesPtr& DeviceResources, const std::vector<DirectXGame::VertexPositionColor>& Vertices, const std::vector<unsigned>& Indices);

	typedef ComPtr<ID3D11Buffer> VertexBuffer;
	typedef ComPtr<ID3D11Buffer> IndexBuffer;

	inline const VertexBuffer& Vertices() const { return vertices; }
	inline const IndexBuffer& Indices() const { return indices; }

	inline size_t VertexCount() const { return vertexCount; }
	inline size_t IndexCount() const { return indexCount; }

protected:
	VertexBuffer vertices;
	IndexBuffer indices;
	size_t vertexCount;
	size_t indexCount;

	DX::DeviceResourcesPtr deviceResources;

	static const unsigned VertexStride; //The stride of each vertex. Used for input layouts
};