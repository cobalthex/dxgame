#include "Pch.hpp"
#include "MeshOps.hpp"

std::vector<unsigned> MeshOps::TriangulatePolygon(const std::vector<Math::Vector3> Vertices, size_t Start, size_t Count)
{
	std::vector<unsigned> indices;

	//Nothing to triangulate
	if (Start >= Vertices.size())
		return indices;

	if (Count == 0)
		Count = Vertices.size();


	return indices;
}