#pragma once

#include "Pch.hpp"
#include "Engine/Common/Box.hpp"
#include "Engine/Graphics/Material.hpp"

//3D boundaries for models
struct Bounds
{
	Box3 box;
	float xyRadius;
	float radius;
};

//A mesh that is part of a model. Contains information about its vertices, indices, and contains a material. Does not inherit from Mesh
class ModelMesh
{
public:
	ModelMesh() { }
	ModelMesh(const std::string& Name, size_t StartVertex, size_t VertexCount, size_t StartIndex, size_t IndexCount, const Material& Material, const Bounds& Bounds)
		: name(Name), startVertex(StartVertex), vertexCount(VertexCount), startIndex(StartIndex), indexCount(IndexCount), material(Material), bounds(Bounds) { }

	inline size_t StartVertex() const { return startVertex; } //The first vertex of this mesh in the model which owns it
	inline size_t VertexCount() const { return vertexCount; } //The number of vertices in this mesh
	inline size_t StartIndex() const { return startIndex; } //The first index of this mesh in the model which owns it
	inline size_t IndexCount() const { return indexCount; } //The number of indices in this mesh

	std::string name;
	Material material;

	inline Bounds Bounds() const { return bounds; }

	inline void Draw(const ComPtr<ID3D11DeviceContext>& DeviceContext, unsigned BaseVertexLocation = 0) const
	{
		DeviceContext->DrawIndexed((unsigned)indexCount, (unsigned)startIndex, BaseVertexLocation);
	}

protected:
	size_t startVertex, vertexCount;
	size_t startIndex, indexCount;

	::Bounds bounds;
};