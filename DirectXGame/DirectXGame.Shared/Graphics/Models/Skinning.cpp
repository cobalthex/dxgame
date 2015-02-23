#pragma once

#include "Pch.hpp"
#include "Skinning.hpp"

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