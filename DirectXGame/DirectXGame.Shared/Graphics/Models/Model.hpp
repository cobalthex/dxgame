#pragma once

#include "Pch.hpp"
#include "Mesh.hpp"
#include "Bone.hpp"
#include "Common/DeviceResources.hpp"
#include "Graphics/ShaderStructures.hpp"

//The primitive topologies available. Does not distinguish between points, lines, or triangles
enum class PrimitiveTopology
{
	Unknown,
	List,
	Strip,
};

//A basic model. Contains a group of meshes and a skeleton
class Model
{
public:
	typedef DirectXGame::VertexPositionNormalTexture VertexType;

	Model()
		: meshes(), bones(), deviceResources(nullptr), vertices(nullptr), indices(nullptr), vertexCount(0), indexCount(0), topology(PrimitiveTopology::Unknown) { }
	Model
	(
		const DX::DeviceResourcesPtr& DeviceResources,
		PrimitiveTopology Topology,
		const std::vector<VertexType>& Vertices, 
		const std::vector<unsigned>& Indices,
		const std::vector<Mesh>& Meshes,
		const std::vector<Bone>& Bones
	);

	std::vector<Mesh> meshes;
	std::vector<Bone> bones; //a collection of all of the bones in the mesh
	Bone* root; //the root bone (null if bones is empty)

	typedef ComPtr<ID3D11Buffer> VertexBuffer;
	typedef ComPtr<ID3D11Buffer> IndexBuffer;

	inline const VertexBuffer& Vertices() const { return vertices; }
	inline const IndexBuffer& Indices() const { return indices; }

	inline size_t VertexCount() const { return vertexCount; }
	inline size_t IndexCount() const { return indexCount; }
	inline PrimitiveTopology Topology() const { return topology; }

	static const unsigned VertexStride() { return vertexStride; }

	void Draw() const; //Draws the mesh using default values. Does not set any shaders, input states, etc

protected:
	DX::DeviceResourcesPtr deviceResources;

	VertexBuffer vertices;
	IndexBuffer indices;
	size_t vertexCount;
	size_t indexCount;
	PrimitiveTopology topology;

	static const unsigned vertexStride; //The stride of each vertex. Used for rendering
};