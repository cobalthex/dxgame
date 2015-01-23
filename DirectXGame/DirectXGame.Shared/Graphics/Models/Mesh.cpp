#include "Pch.hpp"
#include "Mesh.hpp"
#include "Common/DirectXHelper.hpp"

const unsigned Mesh::VertexStride = sizeof(DirectXGame::VertexPositionColor);

Mesh::Mesh(const DX::DeviceResourcesPtr& DeviceResources, const std::vector<DirectXGame::VertexPositionColor>& Vertices, const std::vector<unsigned>& Indices)
	: vertexCount(Vertices.size()), indexCount(Indices.size()), deviceResources(DeviceResources)
{
	//create vertex buffer
	CD3D11_BUFFER_DESC vertexBufferDesc (Vertices.size() * sizeof(DirectXGame::VertexPositionColor), D3D11_BIND_VERTEX_BUFFER);

	D3D11_SUBRESOURCE_DATA vertexBufferData = { 0 };
	vertexBufferData.pSysMem = Vertices.data();
	vertexBufferData.SysMemPitch = 0;
	vertexBufferData.SysMemSlicePitch = 0;

	DX::ThrowIfFailed(deviceResources->GetD3DDevice()->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &vertices));

	//create index buffer
	CD3D11_BUFFER_DESC indexBufferDesc (Indices.size() * sizeof(unsigned), D3D11_BIND_INDEX_BUFFER);
	
	D3D11_SUBRESOURCE_DATA indexBufferData = { 0 };
	indexBufferData.pSysMem = Indices.data();
	indexBufferData.SysMemPitch = 0;
	indexBufferData.SysMemSlicePitch = 0;

	DX::ThrowIfFailed(deviceResources->GetD3DDevice()->CreateBuffer(&indexBufferDesc, &indexBufferData, &indices));
}