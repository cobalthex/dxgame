#pragma once

#include "Sequence.hpp"
#include "Common/EventQueue.hpp"

enum class TimelineEventType
{
	TimelineCompletion,
	AnimationCompletion,
	AnimationKeyframe
};

//Events for the timeline
struct TimelineEvent : public Event
{
	TimelineEventType type;

	Timeline* const timeline;
	Sequence* const animation; //may be null

	TimelineEvent(TimelineEventType Type, Timeline* const Timeline, Sequence* const Sequence = nullptr, const BaseEventArgs& EvArgs = BaseEventArgs())
		: type(Type), timeline(Timeline), animation(Sequence), Event(EvArgs) { }
};