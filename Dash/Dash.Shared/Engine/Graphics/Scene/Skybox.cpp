#include "Pch.hpp"
#include "Skybox.hpp"
#include "Engine/Graphics/Models/VertexFormats.hpp"

using namespace Math;

Skybox::Skybox(const DeviceResourcesPtr& DeviceResources, float S)
{
	std::vector<Vertexes::PositionTextureVertex> vertices =
	{
		Vertexes::PositionTextureVertex({ -S, -S, -S }, { 0, 0 }),
		Vertexes::PositionTextureVertex({ -S, S, -S }, { 1, 0 }),
		Vertexes::PositionTextureVertex({ S, S, -S }, { 0, 1 }),
		Vertexes::PositionTextureVertex({ S, -S, -S }, { 1, 1 }),
		Vertexes::PositionTextureVertex({ -S, -S, S }, { 0, 1 }),
		Vertexes::PositionTextureVertex({ -S, S, S }, { 1, 0 }),
		Vertexes::PositionTextureVertex({ S, S, S }, { 0, 0 }),
		Vertexes::PositionTextureVertex({ S, -S, S }, { 0, 0 }),
	};

	static const std::vector<unsigned> indices =
	{
		0, 2, 1,
		2, 3, 1,
		4, 6, 0,
		6, 2, 0,
		7, 6, 5,
		6, 4, 5,
		3, 7, 1,
		7, 5, 1,
		4, 0, 5,
		0, 1, 5,
		3, 2, 7,
		2, 7, 6,
	};

	box = Mesh::Create(DeviceResources, vertices, indices, PrimitiveTopology::TriangleList, false);
}