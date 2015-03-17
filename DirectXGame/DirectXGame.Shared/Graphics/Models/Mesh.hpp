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

	Mesh() : devContext(nullptr), vertices(nullptr), vertexCount(0), vertexStride(0), indices(nullptr), indexCount(0), topology(PrimitiveTopology::Unknown), hasDynamicUsage(false) { }

	inline const VertexBuffer& Vertices() const { return vertices; }
	inline const IndexBuffer& Indices() const { return indices; }
	inline unsigned VertexCount() const { return vertexCount; }
	inline unsigned IndexCount() const { return indexCount; }
	inline unsigned VertexStride() const { return vertexStride; }
	inline PrimitiveTopology Topology() const { return topology; }
	inline bool HasDynamicUsage() const { return hasDynamicUsage; } //Were the buffers for this created using D3D11_USAGE_DYNAMIC (allows for faster updating if necessary)

	template <typename VertexType>
	void CreateFrom(const DX::DeviceResourcesPtr& DeviceResources, const BasicMesh<VertexType, IndexType>& BasicMesh, bool DynamicUsage = false)
	{
		CreateFrom(DeviceResources, BasicMesh.vertices, BasicMesh.indices, BasicMesh.topology, DynamicUsage);
	}

	template <typename VertexType>
	void CreateFrom(const DX::DeviceResourcesPtr& DeviceResources, const std::vector<VertexType>& Vertices, const std::vector<IndexType>& Indices, PrimitiveTopology Topology, bool DynamicUsage = false)
	{
		devContext = DeviceResources->GetD3DDeviceContext();
		hasDynamicUsage = DynamicUsage;

		//create vertex buffer
		CD3D11_BUFFER_DESC vertexBufferDesc ((unsigned)Vertices.size() * sizeof(VertexType), D3D11_BIND_VERTEX_BUFFER);
		if (DynamicUsage)
		{
			vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		}

		D3D11_SUBRESOURCE_DATA vertexBufferData = { 0 };
		vertexBufferData.pSysMem = Vertices.data();
		vertexBufferData.SysMemPitch = 0;
		vertexBufferData.SysMemSlicePitch = 0;

		Sys::ThrowIfFailed(DeviceResources->GetD3DDevice()->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &vertices));
		vertexStride = (unsigned)sizeof(VertexType);
		vertexCount = (unsigned)Vertices.size();

		topology = Topology;

		//create index buffer
		CD3D11_BUFFER_DESC indexBufferDesc ((unsigned)Indices.size() * sizeof(IndexType), D3D11_BIND_INDEX_BUFFER);
		if (DynamicUsage)
		{
			indexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			indexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		}

		D3D11_SUBRESOURCE_DATA indexBufferData = { 0 };
		indexBufferData.pSysMem = Indices.data();
		indexBufferData.SysMemPitch = 0;
		indexBufferData.SysMemSlicePitch = 0;

		Sys::ThrowIfFailed(DeviceResources->GetD3DDevice()->CreateBuffer(&indexBufferDesc, &indexBufferData, &indices));
		indexCount = (unsigned)Indices.size();
	}
	template <typename VertexType>
	static Mesh Create(const DX::DeviceResourcesPtr& DeviceResources, const BasicMesh<VertexType, IndexType>& BasicMesh, bool DynamicUsage = false)
	{
		return Create(DeviceResources, BasicMesh.vertices, BasicMesh.indices, BasicMesh.topology, DynamicUsage);
	}
	template <typename VertexType>
	static Mesh Create(const DX::DeviceResourcesPtr& DeviceResources, const std::vector<VertexType>& Vertices, const std::vector<IndexType>& Indices, PrimitiveTopology Topology, bool DynamicUsage = false)
	{
		Mesh m;
		m.CreateFrom(DeviceResources, Vertices, Indices, Topology, DynamicUsage);
		return m;
	}

	//Draw the mesh (automatically binds if index >= 0)
	inline virtual void Draw(unsigned Slot = 0) const
	{
		if (Slot >= 0)
			Bind(Slot);

		devContext->DrawIndexed(indexCount, 0, 0);
	}

	//Apply the vertex and index buffers
	inline virtual void Bind(unsigned Slot = 0) const
	{
		static const UINT offset = 0; 
		devContext->IASetVertexBuffers(Slot, 1, vertices.GetAddressOf(), &vertexStride, &offset);
		devContext->IASetIndexBuffer(indices.Get(), DXGI_FORMAT_R32_UINT, 0);
	}

	//Update the all of the mesh's vertices with new data
	template <typename VertexType>
	void UpdateVertices(const std::vector<VertexType>& Vertices)
	{
		if (hasDynamicUsage)
		{
			auto vt = vertices.Get();
			D3D11_MAPPED_SUBRESOURCE map;
			devContext->Map(vt, 0, D3D11_MAP_WRITE_DISCARD, 0, &map);
			CopyMemory(map.pData, Vertices.data(), Vertices.size() * sizeof(VertexType));
			devContext->Unmap(vt, 0);
		}
		else
			devContext->UpdateSubresource(vertices.Get(), 0, nullptr, Vertices.data(), Vertices.size() * sizeof(VertexType), 0);
	}
	//Update the all of the mesh's indices with new data
	void UpdateIndices(const std::vector<IndexType>& Indices)
	{
		if (hasDynamicUsage)
		{
			auto ix = indices.Get();
			D3D11_MAPPED_SUBRESOURCE map;
			devContext->Map(ix, 0, D3D11_MAP_WRITE_DISCARD, 0, &map);
			CopyMemory(map.pData, Indices.data(), Indices.size() * sizeof(IndexType));
			devContext->Unmap(ix, 0);
		}
		else
			devContext->UpdateSubresource(indices.Get(), 0, nullptr, Indices.data(), Indices.size() * sizeof(IndexType), 0);
	}

protected:
	ComPtr<ID3D11DeviceContext> devContext;

	VertexBuffer vertices;
	unsigned vertexCount;
	unsigned vertexStride;

	PrimitiveTopology topology;

	IndexBuffer indices;
	unsigned indexCount;

	bool hasDynamicUsage;
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