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
	if (meshes.size() < 1)
		return;

	Bind(Slot);

	auto sh = (Shaders::LitShader*)meshes.begin()->second.material.shader.get();
	if (sh != nullptr)
		sh->Apply();

	//Draw all of the meshes
	for (auto& m : meshes)
	{
		if (m.second.material.shader != nullptr)
		{
			sh->material.data = m.second.material;
			sh->Update();
		}
		if (m.second.material.diffuseMap != nullptr)
			m.second.material.diffuseMap->Apply();

		//Draw the objects.
		devContext->DrawIndexed((unsigned)m.second.IndexCount(), (unsigned)m.second.StartIndex(), 0);
	}
}