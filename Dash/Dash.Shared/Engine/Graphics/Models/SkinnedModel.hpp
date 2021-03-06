#pragma once

#include "Pch.hpp"
#include "Skinning.hpp"
#include "Engine/Animation/Timeline.hpp"
#include "Engine/Graphics/DeviceResources.hpp"
#include "Engine/Graphics/Shaders/ShaderStructures.hpp"
#include "VertexFormats.hpp"
#include "Model.hpp"

//An animated model. Contains a group of meshes and a skeleton
class SkinnedModel : public Model
{
public:
	typedef Vertexes::SkinnedVertex VertexType;
	typedef unsigned IndexType;
	
	SkinnedModel() = default;
	SkinnedModel
	(
		const DeviceResourcesPtr& DeviceResources,
		const std::vector<VertexType>& Vertices, 
		const std::vector<IndexType>& Indices,
		PrimitiveTopology Topology,
		const std::map<std::string, ModelMesh>& Meshes,
		const std::vector<Joint>& Joints,
		const std::map<std::string, SkinnedSequence> Poses
	);

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

	inline void AddPoseToTimeline(Timeline& Timeline, const std::string& Pose) { Timeline.Add(&poses[Pose]); }
	inline void AddCurrentPoseToTimeline(Timeline& Timeline) { Timeline.Add(&poses[pose]); }

	inline BasicMesh<Vertexes::PositionColorVertex, unsigned> CreateSkeletalMesh(const Color& VertexColor = Color(255, 0, 0)) const { return CreateSkeletalMesh(pose, VertexColor); }
	BasicMesh<Vertexes::PositionColorVertex, unsigned> CreateSkeletalMesh(const std::string& Pose, const Color& VertexColor = Color(255, 0, 0)) const; //create a mesh skeleton using the joints of this model. These meshes use the VertexPositionColor. Returns a triangle list vertex buffer
};