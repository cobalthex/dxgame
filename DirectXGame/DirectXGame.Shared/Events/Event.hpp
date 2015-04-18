#pragma once

#include "Pch.hpp"
#include "Common/Time.hpp"

//Basic settings passed to events
struct BaseEventArgs
{
	TimePoint time;
	::TimeSpan delay;
	unsigned repetitions;

	BaseEventArgs() : delay(::TimeSpan::zero()), repetitions(0), time(TimePoint()) { }
	BaseEventArgs(const ::TimeSpan& Delay, unsigned Repetitions = 0, const TimePoint& Time = Timer::now())
		: time(Time), delay(Delay), repetitions(Repetitions) { }
};

//The base for all events. All should inherit from this
class Event
{
public:
	Event(const BaseEventArgs& Args = BaseEventArgs())
		: time(Args.time), delay(Args.delay), repetitions(Args.repetitions) { }
	virtual ~Event() { }

	TimePoint time;
	::TimeSpan delay; //how long to delay this Event before calling; Works in conjunction with repetitions
	int repetitions; //how many times should this Event be called before its removed
};
