#pragma once

#include "Pch.hpp"
#include "Sequence.hpp"
#include "TimelineEvents.hpp"
#include "Animation.hpp"

//A timeline to manage animations. Does not own the animations
class Timeline : public Animation
{
public:

	Timeline(bool DeleteOnDestroy = true, bool IsLooping = false, const ::TimeSpan& LoopStart = TimeType::zero());
	~Timeline();

	Timeline& operator = (Timeline&& Other);

	void Add(Sequence* Sequence);
	template <class SequenceType = Sequence>
	SequenceType* const AddNew() //create a new sequence and add it to the stack (returns it for editing)
	{
		SequenceType* s = new SequenceType;
		Add(s);
		return s;
	}
	bool Remove(Sequence* Sequence, bool Delete = false);

	inline const std::vector<Sequence*>& Sequences() const { return sequences; }

	//Update all of the animations in the timeline
	void Update();

	void Clear(bool Delete);

	float AnimationPercent(Sequence* Sequence) const; //Get the current frame (returns current frame # + 0-1)

	EventQueue<TimelineEvent> events; //The event handler for the timeline
	bool processEvents = true; //should this timeline process events (defaults to true, use no for manual management)

	bool AtEnd() const override;

	bool deleteOnDestroy = false; //delete all animations when this object is destroyed

protected:
	bool calledFinish = false;

	std::vector<Sequence*> sequences;

	size_t GetCurrentKeyframe(Sequence* Sequence) const;

	TimeType Duration() const override;

	//Searching forward, returning the current keyframe
	float KeyframePercent(const Keyframe& CurrentFrame, const Keyframe& NextFrame) const; //get the percent between two frames of an animation (0 to 1)
};