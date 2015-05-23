#pragma once

#include "Models/Mesh.hpp"
#include "Engine/Common/Math.hpp"
#include "Engine/Graphics/Shaders/PositionColorShader.hpp"

using namespace DirectX::Math;
using PCSVertex = Shaders::PositionColorShader::Vertex;

namespace Primitives
{
	//Create a Width x Height quad on the x y plane.
	//Length: The size of the first axis (usually x or y)
	//Width: The size of the second axis (usually y or z)
	//Z: An optional value for the z coordinate, defaults to 0
	//Transform: an optional transform for each vertex (translation, rotation, scale)
	//Returns a basic mesh with positions
	BasicMesh<Vector3> CreateQuad(float Z = 0, const Matrix& Transform = Matrix::Identity)
	{
		BasicMesh<Vector3> mesh;

		//vertices
		//0 -- 1
		//|    |
		//3 -- 2
		mesh.vertices.push_back(Vector3::Transform(Vector3(-0.5f, -0.5f, Z), Transform));
		mesh.vertices.push_back(Vector3::Transform(Vector3(0.5f, -0.5f, Z), Transform));
		mesh.vertices.push_back(Vector3::Transform(Vector3(0.5f, 0.5f, Z), Transform));
		mesh.vertices.push_back(Vector3::Transform(Vector3(-0.5f, 0.5f, Z), Transform));

		//indices
		mesh.indices.push_back(0);
		mesh.indices.push_back(1);
		mesh.indices.push_back(3);
		mesh.indices.push_back(2);

		mesh.topology = PrimitiveTopology::TriangleStrip;

		return mesh;
	}

	//Create the typical 3 axis lines (each vertex is of length 1)
	BasicMesh<PCSVertex> CreateAxes(const Color& XColor = Color(1, 0, 0), const Color& YColor = Color(0, 1, 0), const Color& ZColor = Color(0, 0, 1))
	{
		BasicMesh<PCSVertex> mesh;

		//vertices
		//Y  Z
		//| /
		//+--X
		PCSVertex v;
		v.color = XColor;
		v.position = Vector3(0, 0, 0);
		mesh.vertices.push_back(v);
		v.position = Vector3(1, 0, 0);
		mesh.vertices.push_back(v);

		v.color = YColor;
		v.position = Vector3(0, 0, 0);
		mesh.vertices.push_back(v);
		v.position = Vector3(0, 1, 0);
		mesh.vertices.push_back(v);

		v.color = ZColor;
		v.position = Vector3(0, 0, 0);
		mesh.vertices.push_back(v);
		v.position = Vector3(0, 0, 1);
		mesh.vertices.push_back(v);

		//indices
		mesh.indices.push_back(0);
		mesh.indices.push_back(1);
		mesh.indices.push_back(2);
		mesh.indices.push_back(3);
		mesh.indices.push_back(4);
		mesh.indices.push_back(5);

		mesh.topology = PrimitiveTopology::LineList;

		return mesh;
	}
}