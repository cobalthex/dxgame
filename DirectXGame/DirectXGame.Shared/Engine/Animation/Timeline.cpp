#include "Pch.hpp"
#include "Timeline.hpp"

Timeline::Timeline(bool DeleteOnDestroy, bool IsLooping, const TimeSpan& LoopStart)
	: deleteOnDestroy(DeleteOnDestroy), Animation(IsLooping, LoopStart)
{
}

Timeline& Timeline::operator = (Timeline&& Other)
{
	if (&Other != this)
	{
		Animation::operator=(Other);
		sequences = std::move(Other.sequences);
		deleteOnDestroy = Other.deleteOnDestroy;
		calledFinish = Other.calledFinish;
		events = std::move(Other.events);
	}
	return *this;
}

Timeline::~Timeline()
{
	Clear(deleteOnDestroy);
}

void Timeline::Add(Sequence* Sequence)
{
	sequences.push_back(Sequence);
}
bool Timeline::Remove(Sequence* Sequence, bool Delete)
{
	auto fnd = std::find(sequences.begin(), sequences.end(), Sequence);
	if (fnd != sequences.end())
	{
		sequences.erase(fnd);
		if (Delete)
			delete Sequence;
		return true;
	}
	return false;
}
void Timeline::Clear(bool Delete)
{
	if (Delete)
	{
		for (auto& i : sequences)
			delete i;
	}
	sequences.clear();
}

size_t Timeline::GetCurrentKeyframe(Sequence* Sequence) const
{
	size_t nFr = Sequence->keyframes.size();
	auto time = TimePoint(Time());
	for (size_t i = nFr - 1; i > 0; i--)
	{
		if (Sequence->keyframes[i].start < time)
			return i;
	}
	return 0;
}

void Timeline::Update()
{
	if (!IsRunning())
		return;

	auto time = Time();
	auto tt = TimePoint(time);
	auto d = Duration();

	bool isDone = true;
	for (auto& i : sequences)
	{

		//no frames, no sequence
		if (i->keyframes.size() < 1)
			continue;

		//don't animate when not in frame (not yet reached)
		if (i->keyframes[0].start > tt)
		{
			isDone = false;
			continue;
		}
		if (i->keyframes[i->keyframes.size() - 1].start < tt) //at end
		{
			i->Finish();
			if (!i->calledComplete)
			{
				events.Enqueue(TimelineEvent(TimelineEventType::AnimationCompletion, this, i));
				i->calledComplete = true;
			}
			continue;
		}
		i->calledComplete = false;
		isDone = false;

		//get current and next frames
		size_t cfr = GetCurrentKeyframe(i);
		size_t nfr = (cfr + 1) % i->keyframes.size();

		i->Update(cfr, nfr, KeyframePercent(i->keyframes[cfr], i->keyframes[nfr]));

		//moved to the next keyframe
		if (i->lastKeyframe != cfr)
		{
			events.Enqueue(TimelineEvent(TimelineEventType::AnimationKeyframe, this, i));
			i->lastKeyframe = cfr;
		}
	}

	if (isDone && !calledFinish)
	{
		events.Enqueue(TimelineEvent(TimelineEventType::TimelineCompletion, this));
		calledFinish = true;
	}
	else if (!isDone)
		calledFinish = false;

	if (isLooping && isDone)
		MoveTo(loopStart);

	if (processEvents)
		events.Cycle();
}

bool Timeline::AtEnd() const
{
	auto time = Time();
	//TODO: add lifetime management
	return (time >= Duration());
}
TimeSpan Timeline::Duration() const
{
	TimePoint max = TimePoint();
	for (auto& i : sequences)
	{
		if (i->keyframes.size() > 0 && i->keyframes.back().start > max)
			max = i->keyframes.back().start;
	}
	return std::chrono::duration_cast<TimeType>(max - TimePoint()); //convert to duration
}

float Timeline::KeyframePercent(const Keyframe& CurrentFrame, const Keyframe& NextFrame) const
{
	auto tc = std::chrono::duration_cast<TimeType>(TimePoint(Time()) - CurrentFrame.start).count();
	auto max = std::chrono::duration_cast<TimeType>(NextFrame.start - CurrentFrame.start).count();

	return (tc / (float)max);
}
float Timeline::AnimationPercent(Sequence* Sequence) const
{
	size_t cfr = GetCurrentKeyframe(Sequence);
	Keyframe& cur = Sequence->keyframes[cfr];
	Keyframe& nxt = Sequence->keyframes[(cfr + 1) % Sequence->keyframes.size()];

	return cfr + KeyframePercent(cur, nxt);
}