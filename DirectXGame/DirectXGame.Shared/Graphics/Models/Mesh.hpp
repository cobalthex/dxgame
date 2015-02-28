#pragma once

#include "Pch.hpp"
#include "Graphics/DeviceResources.hpp"

typedef ComPtr<ID3D11Buffer> VertexBuffer;
typedef ComPtr<ID3D11Buffer> IndexBuffer;
typedef ComPtr<ID3D11Buffer> InstanceBuffer;

//The primitive topologies available
enum class PrimitiveTopology
{
	Unknown,
	List,
	Strip
};

//A mesh that is stored in collections
template <typename VertexType, typename IndexType = unsigned>
struct BasicMesh
{
	std::vector<VertexType> vertices;
	std::vector<IndexType> indices;

	PrimitiveTopology topology;

	BasicMesh() : vertices(), indices(), topology(PrimitiveTopology::Unknown) { }
	BasicMesh(const std::vector<VertexType>& Vertices, const std::vector<IndexType>& Indices, PrimitiveTopology Topology) : vertices(Vertices), indices(Indices), topology(Topology) { }
};

//A simple mesh encapsulating vertex and index buffers. All indices are unsigned integers
class Mesh
{
public:
	typedef unsigned IndexType;

	Mesh() : deviceResources(nullptr), vertices(nullptr), vertexCount(0), vertexStride(0), indices(nullptr), indexCount(0), topology(PrimitiveTopology::Unknown) { }

	inline const VertexBuffer& Vertices() const { return vertices; }
	inline const IndexBuffer& Indices() const { return indices; }
	inline unsigned VertexCount() const { return vertexCount; }
	inline unsigned IndexCount() const { return indexCount; }
	inline unsigned VertexStride() const { return vertexStride; }
	inline PrimitiveTopology Topology() const { return topology; }

	template <typename VertexType>
	void CreateFrom(const DX::DeviceResourcesPtr& DeviceResources, const BasicMesh<VertexType, IndexType>& BasicMesh)
	{
		CreateFrom(DeviceResources, BasicMesh.vertices, BasicMesh.indices, BasicMesh.topology);
	}

	template <typename VertexType>
	void CreateFrom(const DX::DeviceResourcesPtr& DeviceResources, const std::vector<VertexType>& Vertices, const std::vector<IndexType>& Indices, PrimitiveTopology Topology)
	{
		deviceResources = DeviceResources;

		//create vertex buffer
		CD3D11_BUFFER_DESC vertexBufferDesc ((unsigned)Vertices.size() * sizeof(VertexType), D3D11_BIND_VERTEX_BUFFER);

		D3D11_SUBRESOURCE_DATA vertexBufferData = { 0 };
		vertexBufferData.pSysMem = Vertices.data();
		vertexBufferData.SysMemPitch = 0;
		vertexBufferData.SysMemSlicePitch = 0;

		App::ThrowIfFailed(DeviceResources->GetD3DDevice()->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &vertices));
		vertexStride = (unsigned)sizeof(VertexType);
		vertexCount = (unsigned)Vertices.size();

		topology = Topology;

		//create index buffer
		CD3D11_BUFFER_DESC indexBufferDesc((unsigned)Indices.size() * sizeof(IndexType), D3D11_BIND_INDEX_BUFFER);

		D3D11_SUBRESOURCE_DATA indexBufferData = { 0 };
		indexBufferData.pSysMem = Indices.data();
		indexBufferData.SysMemPitch = 0;
		indexBufferData.SysMemSlicePitch = 0;

		App::ThrowIfFailed(DeviceResources->GetD3DDevice()->CreateBuffer(&indexBufferDesc, &indexBufferData, &indices));
		indexCount = (unsigned)Indices.size();
	}
	template <typename VertexType>
	static Mesh Create(const DX::DeviceResourcesPtr& DeviceResources, const BasicMesh<VertexType, IndexType>& BasicMesh)
	{
		return Create(DeviceResources, BasicMesh.vertices, BasicMesh.indices, BasicMesh.topology);
	}
	template <typename VertexType>
	static Mesh Create(const DX::DeviceResourcesPtr& DeviceResources, const std::vector<VertexType>& Vertices, const std::vector<IndexType>& Indices, PrimitiveTopology Topology)
	{
		Mesh m;
		m.CreateFrom(DeviceResources, Vertices, Indices, Topology);
		return m;
	}

	//Draw the mesh (automatically binds if index >= 0)
	inline virtual void Draw(unsigned Slot = 0) const
	{
		if (Slot >= 0)
			Bind(Slot);

		deviceResources->GetD3DDeviceContext()->DrawIndexed(indexCount, 0, 0);
	}

	//Apply the vertex and index buffers
	inline virtual void Bind(unsigned Slot = 0) const
	{
		auto cxt = deviceResources->GetD3DDeviceContext();

		static const UINT offset = 0; 
		cxt->IASetVertexBuffers(Slot, 1, vertices.GetAddressOf(), &vertexStride, &offset);
		cxt->IASetIndexBuffer(indices.Get(), DXGI_FORMAT_R32_UINT, 0);
	}

protected:
	DX::DeviceResourcesPtr deviceResources;

	VertexBuffer vertices;
	unsigned vertexCount;
	unsigned vertexStride;

	PrimitiveTopology topology;

	IndexBuffer indices;
	unsigned indexCount;
};
//A simple encapsulation around vertex, index, and instance buffers
class InstancedMesh: public Mesh
{
public:
	InstancedMesh() : Mesh(), instances(nullptr), instanceCount(0) { }

	inline const InstanceBuffer& Instances() const { return instances; }
	inline unsigned InstanceCount() const { return instanceCount; }

protected:
	InstanceBuffer instances;
	unsigned instanceCount;
};