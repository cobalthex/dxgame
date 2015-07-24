#include "Pch.hpp"
#include "Model.hpp"
#include "Engine/Common/PlatformHelpers.hpp"

Model::Model
(
const DeviceResourcesPtr& DeviceResources,
const std::vector<VertexType>& Vertices,
const std::vector<IndexType>& Indices,
PrimitiveTopology Topology,
const std::map<std::string, ModelMesh>& Meshes
) :
	deviceResources(DeviceResources),
	meshes(Meshes)
{
	CreateFrom(DeviceResources, Vertices, Indices, Topology);
}

void Model::Draw(unsigned Slot) const
{
	BeginDraw(Slot);

	//Draw all of the meshes
	for (auto& m : meshes)
	{
		//Draw the objects.
		m.second.Draw(devContext);
	}
}