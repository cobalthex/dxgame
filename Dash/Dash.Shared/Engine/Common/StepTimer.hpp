#pragma once

#include <wrl.h>

//Helper class for animation and simulation timing.
class StepTimer
{
public:
	StepTimer() : 
		elapsedTicks(0),
		totalTicks(0),
		leftOverTicks(0),
		frameCount(0),
		framesPerSecond(0),
		framesThisSecond(0),
		qpcSecondCounter(0),
		isFixedTimeStep(false),
		targetElapsedTicks(TicksPerSecond / 60)
	{
		if (!QueryPerformanceFrequency(&qpcFrequency))
		{
			throw ref new Platform::FailureException();
		}

		if (!QueryPerformanceCounter(&qpcLastTime))
		{
			throw ref new Platform::FailureException();
		}

		//Initialize max delta to 1/10 of a second.
		qpcMaxDelta = qpcFrequency.QuadPart / 10;
	}

	//Get elapsed time since the previous Update call.
	uint64 GetElapsedTicks() const						{ return elapsedTicks; }
	double GetElapsedSeconds() const					{ return TicksToSeconds(elapsedTicks); }

	//Get total time since the start of the program.
	uint64 GetTotalTicks() const						{ return totalTicks; }
	double GetTotalSeconds() const						{ return TicksToSeconds(totalTicks); }

	//Get total number of updates since start of the program.
	uint32 GetFrameCount() const						{ return frameCount; }

	//Get the current framerate.
	uint32 GetFramesPerSecond() const					{ return framesPerSecond; }

	//Set whether to use fixed or variable timestep mode.
	void SetFixedTimeStep(bool IsFixedTimestep)			{ isFixedTimeStep = IsFixedTimestep; }

	//Set how often to call Update when in fixed timestep mode.
	void SetTargetElapsedTicks(uint64 TargetElapsed)	{ targetElapsedTicks = TargetElapsed; }
	void SetTargetElapsedSeconds(double TargetElapsed)	{ targetElapsedTicks = SecondsToTicks(TargetElapsed); }

	//Integer format represents time using 10,000,000 ticks per second.
	static const uint64 TicksPerSecond = 10000000;

	static double TicksToSeconds(uint64 Ticks)			{ return static_cast<double>(Ticks) / TicksPerSecond; }
	static uint64 SecondsToTicks(double Seconds)		{ return static_cast<uint64>(Seconds * TicksPerSecond); }

	//After an intentional timing discontinuity (for instance a blocking IO operation)
	//call this to avoid having the fixed timestep logic attempt a set of catch-up 
	//Update calls.

	void ResetElapsedTime()
	{
		if (!QueryPerformanceCounter(&qpcLastTime))
		{
			throw ref new Platform::FailureException();
		}

		leftOverTicks = 0;
		framesPerSecond = 0;
		framesThisSecond = 0;
		qpcSecondCounter = 0;
	}

	//Update timer state, calling the specified Update() function the appropriate number of times.
	template<typename TUpdate>
	void Tick(const TUpdate& Update)
	{
		//Query the current time.
		LARGE_INTEGER currentTime;

		if (!QueryPerformanceCounter(&currentTime))
		{
			throw ref new Platform::FailureException();
		}

		uint64 timeDelta = currentTime.QuadPart - qpcLastTime.QuadPart;

		qpcLastTime = currentTime;
		qpcSecondCounter += timeDelta;

		//Clamp excessively large time deltas (e.g. after paused in the debugger).
		if (timeDelta > qpcMaxDelta)
			timeDelta = qpcMaxDelta;

		//Convert QPC units into a canonical tick format. This cannot overflow due to the previous clamp.
		timeDelta *= TicksPerSecond;
		timeDelta /= qpcFrequency.QuadPart;

		uint32 lastFrameCount = frameCount;

		if (isFixedTimeStep)
		{
			//Fixed timestep update logic

			//If the app is running very close to the target elapsed time (within 1/4 of a millisecond) just clamp
			//the clock to exactly match the target value. This prevents tiny and irrelevant errors
			//from accumulating over time. Without this clamping, a game that requested a 60 fps
			//fixed update, running with vsync enabled on a 59.94 NTSC display, would eventually
			//accumulate enough tiny errors that it would drop a frame. It is better to just round 
			//small deviations down to zero to leave things running smoothly.

			if (abs(static_cast<int64>(timeDelta - targetElapsedTicks)) < TicksPerSecond / 4000)
				timeDelta = targetElapsedTicks;

			leftOverTicks += timeDelta;

			while (leftOverTicks >= targetElapsedTicks)
			{
				elapsedTicks = targetElapsedTicks;
				totalTicks += targetElapsedTicks;
				leftOverTicks -= targetElapsedTicks;
				frameCount++;

				Update();
			}
		}
		else
		{
			//Variable timestep update logic.
			elapsedTicks = timeDelta;
			totalTicks += timeDelta;
			leftOverTicks = 0;
			frameCount++;

			Update();
		}

		//Track the current framerate.
		if (frameCount != lastFrameCount)
			framesThisSecond++;

		if (qpcSecondCounter >= static_cast<uint64>(qpcFrequency.QuadPart))
		{
			framesPerSecond = framesThisSecond;
			framesThisSecond = 0;
			qpcSecondCounter %= qpcFrequency.QuadPart;
		}
	}

private:
	//Source timing data uses QPC units.
	LARGE_INTEGER qpcFrequency;
	LARGE_INTEGER qpcLastTime;
	uint64 qpcMaxDelta;

	//Derived timing data uses a canonical tick format.
	uint64 elapsedTicks;
	uint64 totalTicks;
	uint64 leftOverTicks;

	//Members for tracking the framerate.
	uint32 frameCount;
	uint32 framesPerSecond;
	uint32 framesThisSecond;
	uint64 qpcSecondCounter;

	//Members for configuring fixed timestep mode.
	bool isFixedTimeStep;
	uint64 targetElapsedTicks;
};