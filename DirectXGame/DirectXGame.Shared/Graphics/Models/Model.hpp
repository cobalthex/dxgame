#pragma once

#include "Pch.hpp"
#include "Mesh.hpp"
#include "Skinning.hpp"
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
	typedef DirectXGame::VertexSkinned VertexType;


	Model()
		: meshes(), joints(), devContext(nullptr), vertices(nullptr), indices(nullptr), vertexCount(0), indexCount(0), topology(PrimitiveTopology::Unknown) { }
	Model
	(
		const DX::DeviceResourcesPtr& DeviceResources,
		PrimitiveTopology Topology,
		const std::vector<VertexType>& Vertices, 
		const std::vector<unsigned>& Indices,
		const std::vector<Mesh>& Meshes,
		const std::vector<Joint>& Joints
	);

	std::vector<Mesh> meshes; //All of the meshes that make up this model
	std::vector<Joint> joints; //All of the joints connecting the meshes in this model
	std::vector<Pose> poses; //A collection of poses 

	typedef ComPtr<ID3D11Buffer> VertexBuffer;
	typedef ComPtr<ID3D11Buffer> IndexBuffer;

	inline const VertexBuffer& Vertices() const { return vertices; }
	inline const IndexBuffer& Indices() const { return indices; }

	inline size_t VertexCount() const { return vertexCount; }
	inline size_t IndexCount() const { return indexCount; }
	inline PrimitiveTopology Topology() const { return topology; }

	static const unsigned VertexStride() { return vertexStride; }

	void Bind(unsigned Slot = 0) const; //Binds vertex/index data to be drawn. This is automatically called by Draw()
	void Draw(unsigned Slot = 0) const; //Draws only the basic mesh without any materials or skinning

protected:
	ComPtr<ID3D11DeviceContext> devContext;

	VertexBuffer vertices;
	IndexBuffer indices;
	size_t vertexCount;
	size_t indexCount;
	PrimitiveTopology topology;

	static const unsigned vertexStride; //The stride of each vertex. Used for rendering
};