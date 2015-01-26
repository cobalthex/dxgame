#pragma once

#include "Pch.hpp"
#include "Material.hpp"

//A single mesh that contains a material and the information for rendering the vertices
class Mesh
{
public:
	Mesh() : startVertex(0), vertexCount(0), startIndex(0), indexCount(0), material() { }
	Mesh(size_t StartVertex, size_t VertexCount, size_t StartIndex, size_t IndexCount, const Material& Material)
		: material(Material), startVertex(StartVertex), vertexCount(VertexCount), startIndex(StartIndex), indexCount(IndexCount) { }

	inline size_t StartVertex() const { return startVertex; } //The first vertex of this mesh in the model which owns it
	inline size_t VertexCount() const { return vertexCount; } //The number of vertices in this mesh
	inline size_t StartIndex() const { return startIndex; } //The first index of this mesh in the model which owns it
	inline size_t IndexCount() const { return indexCount; } //The number of indices in this mesh

	Material material;

protected:
	size_t startVertex, vertexCount;
	size_t startIndex, indexCount;
};