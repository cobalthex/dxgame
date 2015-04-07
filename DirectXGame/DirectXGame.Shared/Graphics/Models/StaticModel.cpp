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
	if (meshes.size() < 1)
		return;

	Bind(Slot);

	auto sh = (Shaders::LitShader*)meshes[0].material.shader.get();
	sh->Apply();

	//Draw all of the meshes
	for (auto& m : meshes)
	{
		if (m.material.shader != nullptr)
		{
			sh->material.data = m.material;
			sh->Update();
		}
		if (m.material.diffuseMap != nullptr)
			m.material.diffuseMap->Apply();

		//Draw the objects.
		devContext->DrawIndexed((unsigned)m.IndexCount(), (unsigned)m.StartIndex(), 0);
	}
}