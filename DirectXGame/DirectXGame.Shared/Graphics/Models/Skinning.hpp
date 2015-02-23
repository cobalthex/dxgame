#pragma once

#include "Common/SimpleMath.hpp"
#include "Animation/Sequence.hpp"

using namespace DirectX::SimpleMath;

//A single joint in a skeleton of a mesh
struct Joint
{
public:
	std::string name; //an identifyable name of this joint

	Quaternion rotation;
	Vector3 translation;
	Vector3 scale;

	size_t index; //the index of this jointin the model's collection
	ptrdiff_t parent; //The parent index, negative if none
};

//Contains all of the transforms of a skeleton in one frame of a pose
struct Pose
{
	Quaternion* rotations = nullptr;
	Vector3* translations = nullptr;
	Vector3* scales = nullptr;

	Pose() { }
};

//The animation of a single pose
class SkinnedSequence : public Sequence
{
public:
	SkinnedSequence() : poses(), pose(), numJoints(0), Sequence() { }
	SkinnedSequence(const std::vector<Pose>& Poses, size_t NumJoints) : poses(Poses), pose(Poses.front()), numJoints(NumJoints) { }
	inline SkinnedSequence& operator = (const SkinnedSequence& Sequence)
	{
		if (&Sequence == this)
			return *this;

		Sequence::operator =(Sequence);

		poses = Sequence.poses;
		pose = Sequence.pose;

		numJoints = Sequence.numJoints;
	
		return *this;
	}

	std::vector<Pose> poses; //all of the poses in this sequence, one for each keyframe
	Pose pose; //the active pose

	inline size_t NumJoints() const { return numJoints; }

	virtual void Update(size_t CurrentFrame, size_t NextFrame, float FramePercent) override; //Updates the pose. Uses Slerp and Lerp

private:
	size_t numJoints; //The total number of joints in these pose
};