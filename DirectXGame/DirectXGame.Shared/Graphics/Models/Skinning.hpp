#pragma once

#include "Common/SimpleMath.hpp"
#include "Animation/Sequence.hpp"

using namespace DirectX::SimpleMath;

//A single joint in a skeleton of a mesh
//Contains a pre-calculated matrix based on the rotation, scale, and translation
struct Joint
{
public:
	std::string name = ""; //an identifyable name of this joint

	Matrix transform = Matrix(); //thre pre-calculated transform of this joint
	Matrix inverseTransform = Matrix(); //the pre-calculated inverse-transform of this joint; = inv(transform)

	size_t index = 0; //the index of this jointin the model's collection
	ptrdiff_t parent = -1; //The parent index, negative if none
};

//Contains all of the transforms of a skeleton in one frame of a pose
struct Pose
{
	Pose() { }
	Pose(size_t Count) : rotations(new Quaternion[Count]), translations(new Vector3[Count]), scales(new Vector3[Count]), count(Count) { }
	~Pose()
	{
		if (rotations != nullptr)
			delete[] rotations;
		if (translations != nullptr)
			delete[] translations;
		if (scales != nullptr)
			delete[] scales;
	}
	Pose(const Pose& Copy)
		: Pose(Copy.count)
	{
		std::copy(Copy.rotations, Copy.rotations + count, rotations);
		std::copy(Copy.translations, Copy.translations + count, translations);
		std::copy(Copy.scales, Copy.scales + count, scales);
	}
	Pose& operator = (const Pose& Copy);

	Quaternion* rotations = nullptr;
	Vector3* translations = nullptr;
	Vector3* scales = nullptr;

	inline size_t Count() const { return count; }

protected:
	size_t count = 0;
};

//The animation of a single pose
class SkinnedSequence : public Sequence
{
public:
	SkinnedSequence() : poses(), pose(), numJoints(0), Sequence() { }
	SkinnedSequence(const std::vector<Pose>& Poses, size_t NumJoints) : poses(Poses), pose(), numJoints(NumJoints)
	{
		if (poses.size() > 0)
			pose = poses.front();
	}
	SkinnedSequence& operator = (const SkinnedSequence& Sequence);

	std::vector<Pose> poses; //all of the poses in this sequence, one for each keyframe
	Pose pose; //the active pose

	inline size_t NumJoints() const { return numJoints; }

	virtual void Update(size_t CurrentFrame, size_t NextFrame, float FramePercent) override; //Updates the pose. Uses Slerp and Lerp

private:
	size_t numJoints; //The total number of joints in these pose
};