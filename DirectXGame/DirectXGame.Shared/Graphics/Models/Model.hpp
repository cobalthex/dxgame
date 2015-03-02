#pragma once

#include "Pch.hpp"
#include "Skinning.hpp"
#include "Graphics/DeviceResources.hpp"
#include "Graphics/ShaderStructures.hpp"
#include "ModelMesh.hpp"

//A basic model. Contains a group of meshes and a skeleton
class Model : public Mesh
{
public:
	typedef VertexTypes::VertexSkinned VertexType;
	typedef unsigned IndexType;

	Model()
		: meshes(), joints(), poses(), devContext(nullptr), Mesh() { }
	Model
	(
		const DX::DeviceResourcesPtr& DeviceResources,
		const std::vector<VertexType>& Vertices, 
		const std::vector<IndexType>& Indices,
		PrimitiveTopology Topology,
		const std::vector<ModelMesh>& Meshes,
		const std::vector<Joint>& Joints,
		const std::map<std::string, SkinnedSequence> Poses
	);

	std::vector<ModelMesh> meshes; //All of the model meshes that make up this model
	std::vector<Joint> joints; //All of the joints connecting the meshes in this model
	std::map<std::string, SkinnedSequence> poses; //A collection of animated poses
	std::string pose; //the current pose (maps to poses)

	inline void Skin(Matrix* PoseArray, size_t MaxPoses = MAX_JOINTS) const { Skin(pose, PoseArray, MaxPoses); } //bind the current pose to a constant buffer
	void Skin(const std::string& Pose, Matrix* PoseArray, size_t MaxPoses = MAX_JOINTS) const; //Bind a pose to a constant buffer (uses bind pose if doesn't exist)

	void Draw(unsigned Slot = 0) const; //Draws only the basic mesh without any materials or skinning

	inline BasicMesh<VertexTypes::VertexPositionColor, unsigned> CreateSkeletalMesh(const Color& VertexColor = Color(255, 0, 0)) const { return CreateSkeletalMesh(pose, VertexColor); }
	BasicMesh<VertexTypes::VertexPositionColor, unsigned> CreateSkeletalMesh(const std::string& Pose, const Color& VertexColor = Color(255, 0, 0)) const; //create a mesh skeleton using the joints of this model. These meshes use the VertexPositionColor. Returns a triangle list vertex buffer

protected:
	DX::DeviceResourcesPtr deviceResources;
	ComPtr<ID3D11DeviceContext> devContext;
};