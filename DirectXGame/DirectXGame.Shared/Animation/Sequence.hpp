#pragma once

#include "Pch.hpp"
#include "Common/Time.hpp"
#include "Events/Event.hpp"

class Timeline;

//Easing types for sequences
enum class Easing
{
	Linear
};

//A single keyframe in an sequence
//Note: all starts are absolute to the timeline/sequence to which they belong
struct Keyframe
{
	Keyframe() : start(), easing(Easing::Linear) { }
	Keyframe(const TimePoint& Start, Easing Easing = Easing::Linear) : start(Start), easing(Easing) { }

	TimePoint start; //start relative to the timeline
	Easing easing; //the easing to use (defaults to linear)

	bool operator < (const Keyframe& Other) { return start < Other.start; } //Does this keyframe go first?
	bool operator > (const Keyframe& Other) { return start > Other.start; } //Does this keyframe go last?
	bool operator == (const Keyframe& Other) { return (start == Other.start && easing == Other.easing); }
};

typedef std::vector<Keyframe> KeyframeList;

//A basic frame-based sequence that is part of (owned by) a timeline
//Keyframes are sorted automatically
class Sequence
{
public:
	Sequence() : keyframes(), lastKeyframe(0), calledComplete(false) { }

	//Reset the sequence to its original state
	inline virtual void Reset() { lastKeyframe = 0; calledComplete = false; }
	//Finish the sequence (similar to reset)
	inline virtual void Finish() { }

	//Shift all of the keyframes in an sequence by Offset
	virtual void Shift(const TimeSpan& Offset);

	inline virtual void Clear() { keyframes.clear(); } //Remove all keyframes from the sequence
	inline virtual void Remove(size_t Index) { keyframes.erase(keyframes.begin() + Index); }
	inline virtual void Remove(const Keyframe& Keyframe) { auto idx = std::find(keyframes.begin(), keyframes.end(), Keyframe); keyframes.erase(idx); }

	inline virtual void Append(const Keyframe& Keyframe) { keyframes.push_back(Keyframe); std::sort(keyframes.begin(), keyframes.end()); } //Append a keyframe to the sequence
	//Append a list of frames to the sequence, use Offset to apply an offset to each of the frames (for example appending a new sequence to the end)
	virtual void Append(const KeyframeList& Keyframes, const TimeSpan& Offset = TimeSpan());
	inline virtual void Append(Sequence* const sequence, const TimeSpan& Offset = TimeSpan()) { Append(sequence->keyframes, Offset); }

	inline virtual void Insert(size_t Index, const Keyframe& Keyframe) { keyframes.insert(keyframes.begin() + Index, Keyframe); }
	virtual void Insert(size_t Index, const KeyframeList& Keyframes, const TimeSpan& Offset = TimeSpan());
	inline virtual void Insert(size_t Index, Sequence* const sequence, const TimeSpan& Offset = TimeSpan()) { Insert(Index, sequence->keyframes, Offset); }

	//Update the sequence based on the current frame, next frame, and the percent (0 to 1) between this frame and next
	inline virtual void Update(size_t CurrentFrame, size_t NextFrame, float FramePercent) { }

	inline const KeyframeList& Keyframes() const { return keyframes; }
	inline KeyframeList& Keyframes() { return keyframes; }

protected:
	KeyframeList keyframes; //the collection of all of the keyframes

private:
	size_t lastKeyframe; //the last keyframe, used for onKeyframe; set by timeline
	bool calledComplete; //has the completion event been called?
	friend class Timeline;
};

template <typename TKey>
using SequenceCollection = std::map<TKey, Sequence>; //A collection of sequences stored by key

//Sequence events
struct SequenceEvent : public Event
{
	Sequence* sequence;
	Timeline& timeline;

	SequenceEvent(Timeline& Timeline, Sequence* const Sequence)
		: sequence(Sequence), timeline(Timeline), Event() { }
};
//An event that occurs when a keyframe of a sequence is triggered
struct KeyframeEvent : public SequenceEvent
{
	Keyframe& keyframe; //the actual keyframe called
	size_t keyframeIndex; //the position of the keyframe in the sequence keyframes list

	KeyframeEvent(Timeline& Timeline, Sequence* const Sequence, Keyframe& Keyframe, size_t KeyframeIndex)
		: SequenceEvent(Timeline, Sequence), keyframe(Keyframe), keyframeIndex(KeyframeIndex) { }
};