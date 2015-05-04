#include "Pch.hpp"
#include "StaticModel.hpp"
#include "Common/PlatformHelpers.hpp"

StaticModel::StaticModel
(
const DeviceResourcesPtr& DeviceResources,
const std::vector<VertexType>& Vertices,
const std::vector<IndexType>& Indices,
PrimitiveTopology Topology,
const std::map<std::string, MeshType>& Meshes
) :
deviceResources(DeviceResources),
devContext(DeviceResources->GetD3DDeviceContext()),
meshes(Meshes)
{
	CreateFrom(DeviceResources, Vertices, Indices, Topology);
}

void StaticModel::Draw(unsigned Slot) const
{
	BeginDraw(Slot);

	//Draw all of the meshes
	for (auto& m : meshes)
	{
		if (m.second.material.shader.get() != Shader::ActiveShader)
			m.second.material.shader->Apply();

		m.second.material.Apply();
		m.second.material.shader->Update();

		//Draw the objects.
		m.second.Draw(devContext);
	}
}