#pragma once

#include "Common/Time.hpp"

//the basic implementation of an animatable system or object
class Animation
{
public:
	Animation(bool IsLooping = false, const ::TimeSpan& LoopStart = ::TimeSpan::zero()) //Create an animation
		: start(Timer::now()), isLooping(IsLooping), loopStart(LoopStart) { }
	virtual ~Animation() = default;

	inline virtual void Start() { isRunning = true; start = Timer::now(); } //Start the animation
	inline virtual void Restart() { Reset(); Start(); } //Restart the animation
	inline virtual void Stop() { isRunning = false; elapsed += std::chrono::duration_cast<TimeType>(Timer::now() - start); } //Stop the animation without resetting
	inline virtual void Reset() { elapsed = ::TimeSpan(); start = Timer::now(); } //Reset the animation to the beginning. Does not affect running state
	inline virtual void MoveTo(const ::TimeSpan& Time) { start = Timer::now(); elapsed = Time; } //Move to a time in the animation
	inline virtual void MoveTo(const TimePoint& Time) { MoveTo(std::chrono::duration_cast<TimeType>(Time - TimePoint())); } //Move to a time in the animation
	inline virtual void Finish() { elapsed = Duration(); start = Timer::now(); }

	inline bool IsRunning() const { return isRunning; } //Is the animation running?
	inline TimePoint StartTime() const { return start; } //Get when the animation started
	virtual ::TimeSpan Duration() const = 0; //The full duration of the animation

	//The total time of the animation (continues past end of animation)
	inline ::TimeSpan Time() const
	{
		if (!isRunning)
			return elapsed;

		auto d = Duration();
		auto time = std::chrono::duration_cast<TimeType>(Timer::now() - start) + elapsed;

		/*if (!isLooping && time >= d)
			return d;*/

		if (isLooping)
			return ::TimeSpan(((size_t)(time.count() * speed) % d.count()));
		
		return ::TimeSpan((size_t)(time.count() * speed));
	}

	//has the animation finished playing (always false if looping)
	inline virtual bool AtEnd() const
	{
		if (isLooping)
			return false;

		return (Time() >= Duration());
	}

	bool isLooping; //Is the animation looping (defaults to false)
	::TimeSpan loopStart; //where to start once looped (relative to the animation)

	float speed = 1.f; //the speed multiplier of animations (1 for default)

private:
	bool isRunning = true; //is the timeline currently active
	::TimeSpan elapsed = ::TimeSpan::zero(); //the difference between the last stop and the last start
	TimePoint start; //the last start time in absolute time of this animation
};