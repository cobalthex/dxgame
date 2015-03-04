#pragma once

#include "Material.hpp"

//3D boundaries for models
struct Bounds
{
	Vector3 boundingBoxMin;
	Vector3 boundingBoxAxis;
	float xyRadius;
	float radius;
};

//A mesh that is part of a model. Contains information about its vertices, indices, and contains a material. Does not inherit from Mesh
class ModelMesh
{
public:
	ModelMesh() : startVertex(0), vertexCount(0), startIndex(0), indexCount(0), material() { }
	ModelMesh(size_t StartVertex, size_t VertexCount, size_t StartIndex, size_t IndexCount, const Material& Material)
		: startVertex(StartVertex), vertexCount(VertexCount), startIndex(StartIndex), indexCount(IndexCount), material(Material) { }

	inline size_t StartVertex() const { return startVertex; } //The first vertex of this mesh in the model which owns it
	inline size_t VertexCount() const { return vertexCount; } //The number of vertices in this mesh
	inline size_t StartIndex() const { return startIndex; } //The first index of this mesh in the model which owns it
	inline size_t IndexCount() const { return indexCount; } //The number of indices in this mesh

	Material material;

protected:
	size_t startVertex, vertexCount;
	size_t startIndex, indexCount;
};