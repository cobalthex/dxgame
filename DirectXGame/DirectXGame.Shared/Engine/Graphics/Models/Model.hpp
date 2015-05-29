#pragma once

#include "Pch.hpp"
#include "Skinning.hpp"
#include "Engine/Graphics/DeviceResources.hpp"
#include "Engine/Graphics/Shaders/LitSkinnedShader.hpp"
#include "Engine/Graphics/Shaders/PositionColorShader.hpp"
#include "ModelMesh.hpp"
#include "Mesh.hpp"

//An animated model. Contains a group of meshes and a skeleton
class Model : public Mesh
{
public:
	typedef Shaders::LitSkinnedShader::Vertex VertexType;
	typedef unsigned IndexType;
	typedef ModelMesh<Materials::LitMaterial> MeshType;
	
	Model() { }
	Model
	(
		const DeviceResourcesPtr& DeviceResources,
		const std::vector<VertexType>& Vertices, 
		const std::vector<IndexType>& Indices,
		PrimitiveTopology Topology,
		const std::map<std::string, MeshType>& Meshes,
		const std::vector<Joint>& Joints,
		const std::map<std::string, SkinnedSequence> Poses
	);

	std::map<std::string, MeshType> meshes; //All of the model meshes that make up this model
	std::vector<Joint> joints; //All of the joints connecting the meshes in this model
	std::map<std::string, SkinnedSequence> poses; //A collection of animated poses
	std::string pose; //the current pose (maps to poses)

	inline void Skin(Matrix* PoseArray, size_t MaxPoses = MAX_JOINTS) const { Skin(pose, PoseArray, MaxPoses); } //bind the current pose to a constant buffer
	void Skin(const std::string& Pose, Matrix* PoseArray, size_t MaxPoses = MAX_JOINTS) const; //Bind a pose to a constant buffer (uses bind pose if doesn't exist)

	inline void BeginDraw(unsigned Slot = 0) const //Apply the model but do not draw any of the meshes
	{
		if (meshes.size() < 1)
			return;

		Bind(Slot);
	}
	void Draw(unsigned Slot = 0) const; //Draws only the basic mesh without any materials or skinning

	inline BasicMesh<Shaders::PositionColorShader::Vertex, unsigned> CreateSkeletalMesh(const Color& VertexColor = Color(255, 0, 0)) const { return CreateSkeletalMesh(pose, VertexColor); }
	BasicMesh<Shaders::PositionColorShader::Vertex, unsigned> CreateSkeletalMesh(const std::string& Pose, const Color& VertexColor = Color(255, 0, 0)) const; //create a mesh skeleton using the joints of this model. These meshes use the VertexPositionColor. Returns a triangle list vertex buffer

protected:
	DeviceResourcesPtr deviceResources;
	ComPtr<ID3D11DeviceContext> devContext;
};