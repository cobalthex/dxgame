#include "Pch.hpp"
#include "Sequence.hpp"

void Sequence::Shift(const TimeSpan& Offset)
{
	for (unsigned i = 0; i < keyframes.size(); i++)
		keyframes[i].start += Offset;
}

void Sequence::Append(const KeyframeList& Keyframes, const TimeSpan& Offset)
{
	size_t sz = keyframes.size();
	keyframes.insert(keyframes.end(), Keyframes.begin(), Keyframes.end());
	for (size_t i = sz; i < keyframes.size(); i++)
		keyframes[i].start += Offset;

	std::sort(keyframes.begin(), keyframes.end());
}

void Sequence::Insert(size_t Index, const KeyframeList& Keyframes, const TimeSpan& Offset)
{
	keyframes.insert(keyframes.begin() + Index, Keyframes.begin(), Keyframes.end());
	for (size_t i = Index; i < Keyframes.size(); i++)
		keyframes[i].start += Offset;

	std::sort(keyframes.begin(), keyframes.end());
}
