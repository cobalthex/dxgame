#pragma once

#include "Pch.hpp"
#include "Engine/Graphics/DeviceResources.hpp"
#include "Engine/Graphics/Shaders/LitShader.hpp"
#include "ModelMesh.hpp"
#include "Mesh.hpp"

//An static (lit) model. Contains a group of meshes
class StaticModel : public Mesh
{
public:
	typedef Shaders::LitShader::Vertex VertexType;
	typedef unsigned IndexType;
	typedef ModelMesh<Materials::LitMaterial> MeshType;

	StaticModel()
		: meshes(), devContext(nullptr), Mesh() { }
	StaticModel
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
	void Draw(unsigned Slot = 0) const; //Draws only the basic mesh without any materials or skinning

protected:
	DeviceResourcesPtr deviceResources;
	ComPtr<ID3D11DeviceContext> devContext;
};