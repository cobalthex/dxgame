#include "Pch.hpp"
#include "Model.hpp"
#include "Common/PlatformHelpers.hpp"

const unsigned Model::vertexStride = sizeof(VertexType);

Model::Model
(
	const DX::DeviceResourcesPtr& DeviceResources,
	PrimitiveTopology Topology,
	const std::vector<VertexType>& Vertices,
	const std::vector<unsigned>& Indices,
	const std::vector<Mesh>& Meshes,
	const std::vector<Bone>& Bones
) : deviceResources(DeviceResources), vertexCount(Vertices.size()), indexCount(Indices.size()), meshes(Meshes), bones(Bones), topology(Topology)
{
	//create vertex buffer
	CD3D11_BUFFER_DESC vertexBufferDesc(Vertices.size() * vertexStride, D3D11_BIND_VERTEX_BUFFER);

	D3D11_SUBRESOURCE_DATA vertexBufferData = { 0 };
	vertexBufferData.pSysMem = Vertices.data();
	vertexBufferData.SysMemPitch = 0;
	vertexBufferData.SysMemSlicePitch = 0;

	DX::ThrowIfFailed(deviceResources->GetD3DDevice()->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &vertices));

	//create index buffer
	CD3D11_BUFFER_DESC indexBufferDesc(Indices.size() * sizeof(unsigned), D3D11_BIND_INDEX_BUFFER);

	D3D11_SUBRESOURCE_DATA indexBufferData = { 0 };
	indexBufferData.pSysMem = Indices.data();
	indexBufferData.SysMemPitch = 0;
	indexBufferData.SysMemSlicePitch = 0;

	DX::ThrowIfFailed(deviceResources->GetD3DDevice()->CreateBuffer(&indexBufferDesc, &indexBufferData, &indices));
}

void Model::Draw() const
{
	if (vertexCount < 1 || indexCount < 1)
		return; //nothing to draw

	const auto& cxt = deviceResources->GetD3DDeviceContext();

	// Each vertex is one instance of the VertexPositionColor struct.
	UINT offset = 0;
	UINT stride = vertexStride;
	cxt->IASetVertexBuffers(0, 1, vertices.GetAddressOf(), &stride, &offset);

	cxt->IASetIndexBuffer(indices.Get(), DXGI_FORMAT_R32_UINT, 0);

	//Draw all of the meshes
	for (auto& m : meshes)
	{
		// Draw the objects.
		cxt->DrawIndexed(m.IndexCount(), m.StartIndex(), 0);
	}
}