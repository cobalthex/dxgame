#pragma once

#include "Pch.hpp"
#include "Engine/Graphics/DeviceResources.hpp"
#include "Engine/Graphics/Shaders/LitShader.hpp"
#include "ModelMesh.hpp"
#include "Mesh.hpp"

//An static (lit) model. Contains a group of meshes
class Model : public Mesh
{
public:
	typedef Shaders::LitShader::Vertex VertexType;
	typedef unsigned IndexType;
	typedef ModelMesh<Materials::LitMaterial> MeshType;

	Model() = default;
	Model
	(
		const DeviceResourcesPtr& DeviceResources,
		const std::vector<VertexType>& Vertices,
		const std::vector<IndexType>& Indices,
		PrimitiveTopology Topology,
		const std::map<std::string, MeshType>& Meshes
	);

	std::map<std::string, MeshType> meshes; //All of the model meshes that make up this model

	inline void BeginDraw(unsigned Slot = 0) const //Apply the model but do not draw any of the meshes
	{
		if (meshes.size() < 1)
			return;

		Bind(Slot);
	}
	virtual void Draw(unsigned Slot = 0) const; //Draw the model

protected:
	DeviceResourcesPtr deviceResources;

	Model(const DeviceResourcesPtr& DeviceResources)
		: Mesh(DeviceResources), deviceResources(DeviceResources) { }
};