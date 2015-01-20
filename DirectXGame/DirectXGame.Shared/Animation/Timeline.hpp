#pragma once

#include "Pch.hpp"
#include "Sequence.hpp"
#include "TimelineEvents.hpp"
#include "Animation.hpp"

//A timeline to manage animations. Does not own the animations
class Timeline : public Animation
{
public:

	bool deleteOnDestroy = false; //delete all animations when this object is destroyed

	Timeline(bool DeleteOnDestroy = true, bool IsLooping = false, const TimeSpan& LoopStart = TimeType::zero());
	~Timeline();

	void Add(Sequence* const Sequence);
	template <class SequenceType = Sequence>
	SequenceType* const AddNew() //create a new sequence and add it to the stack (returns it for editing)
	{
		SequenceType* s = new SequenceType;
		Add(s);
		return s;
	}
	bool Remove(Sequence* const Sequence, bool Delete = false);

	inline const std::vector<Sequence*>& Sequences() const { return sequences; }

	//Update all of the animations in the timeline
	void Update();

	void Clear(bool Delete);

	float AnimationPercent(Sequence* const Sequence) const; //Get the current frame (returns current frame # + 0-1)

	EventQueue<TimelineEvent> events = EventQueue<TimelineEvent>(); //The event handler for the timeline
	bool processEvents = true; //should this timeline process events (defaults to true, use no for manual management)

	bool AtEnd() const override;

protected:
	bool calledFinish = false;

	std::vector<Sequence*> sequences = std::vector<Sequence*>();

	size_t GetCurrentKeyframe(Sequence* const Sequence) const;

	TimeType Duration() const override;

	//Searching forward, returning the current keyframe
	float KeyframePercent(const Keyframe& CurrentFrame, const Keyframe& NextFrame) const; //get the percent between two frames of an animation (0 to 1)
};