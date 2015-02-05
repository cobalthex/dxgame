#pragma once

#include "Common/SimpleMath.hpp"
#include "Animation/Sequence.hpp"

//contains structures for implementing skinning on models


//A single joint in a skeleton of a mesh
struct Joint
{
public:
	std::string name; //an identifyable name of this joint

	DirectX::SimpleMath::Quaternion rotation;
	DirectX::SimpleMath::Vector3 translation;
	DirectX::SimpleMath::Vector3 scale;

	size_t index; //the index of this bone in the model's collection
	Joint* parent; //The parent bone. Null if root
};

//Contains all of the transforms of a skeleton in one pose
//Each contains n items where n is the number of joints in the owning skeleton
struct Pose
{
public:
	DirectX::SimpleMath::Quaternion* rotations;
	DirectX::SimpleMath::Vector3* translations;
	DirectX::SimpleMath::Vector3* scales;
};

class SkinnedSequence : public Sequence
{
public:
	std::vector<Pose> poses; //all of the poses in this sequence, one for each keyframe
	Pose pose; //the active pose

	virtual void Update(size_t CurrentFrame, size_t NextFrame, float FramePercent) override;
};