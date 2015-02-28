#pragma once

#include "Pch.hpp"
#include "Skinning.hpp"

Pose& Pose::operator = (const Pose& Copy)
{
	if (&Copy == this)
		return *this;

	if (count != Copy.count)
	{
		if (rotations != nullptr)
			delete[] rotations;
		if (translations != nullptr)
			delete[] translations;
		if (scales != nullptr)
			delete[] scales;

		count = Copy.count;

		rotations = new Quaternion[count];
		translations = new Vector3[count];
		scales = new Vector3[count];
	}

	std::copy(Copy.rotations, Copy.rotations + count, rotations);
	std::copy(Copy.translations, Copy.translations + count, translations);
	std::copy(Copy.scales, Copy.scales + count, scales);

	return *this;
}

SkinnedSequence& SkinnedSequence::operator = (const SkinnedSequence& Sequence)
{
	if (&Sequence == this)
		return *this;

	Sequence::operator =(Sequence);

	poses = Sequence.poses;
	pose = Sequence.pose;

	numJoints = Sequence.numJoints;

	return *this;
}

void SkinnedSequence::Update(size_t CurrentFrame, size_t NextFrame, float FramePercent)
{
	auto& cpose = poses[CurrentFrame];
	auto& npose = poses[NextFrame];

	for (size_t i = 0; i < numJoints; i++)
	{
		pose.rotations[i] = Quaternion::Slerp(cpose.rotations[i], npose.rotations[i], FramePercent);
		pose.translations[i] = Vector3::Lerp(cpose.translations[i], npose.translations[i], FramePercent);
		pose.scales[i] = Vector3::Lerp(cpose.scales[i], npose.scales[i], FramePercent);
	}
}