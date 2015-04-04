#include "Pch.hpp"
#include "StaticModel.hpp"
#include "Common/PlatformHelpers.hpp"

StaticModel::StaticModel
(
const DeviceResourcesPtr& DeviceResources,
const std::vector<VertexType>& Vertices,
const std::vector<IndexType>& Indices,
PrimitiveTopology Topology,
const std::vector<MeshType>& Meshes
) :
deviceResources(DeviceResources),
devContext(DeviceResources->GetD3DDeviceContext()),
meshes(Meshes)
{
	CreateFrom(DeviceResources, Vertices, Indices, Topology);
}

void StaticModel::Draw(unsigned Slot) const
{
	Bind(Slot);

	//Draw all of the meshes
	for (auto& m : meshes)
	{
		if (m.material.shader != nullptr)
		{
			m.material.shader->material.data = m.material;
			m.material.shader->Update();
			m.material.shader->Apply();
		}
		if (m.material.diffuseMap != nullptr)
			m.material.diffuseMap->Apply();

		//draw each mesh
		devContext->DrawIndexed((unsigned)m.IndexCount(), (unsigned)m.StartIndex(), 0);
	}
}