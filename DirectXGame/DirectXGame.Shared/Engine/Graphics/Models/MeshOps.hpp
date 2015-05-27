#pragma once

#include "Engine/Common/Math.hpp"

namespace MeshOps
{
	std::vector<unsigned> TriangulatePolygon(const std::vector<Math::Vector3> Vertices, size_t Start = 0, size_t Count = 0); //Triangulate a polygon using ear clipping and return a list of indices (relative to Start). Count is an optional count, use 0 for the rest of the polygon
}