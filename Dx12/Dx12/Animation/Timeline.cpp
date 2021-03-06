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
	for (size_t i = 0; i < sequences.size(); i++)
	{
		auto& s = sequences[i];

		//no frames, no sequence
		if (s->keyframes.size() < 1)
			continue;

		//don't animate when not in frame (not yet reached)
		if (s->keyframes[0].start > tt)
		{
			isDone = false;
			continue;
		}
		if (s->keyframes.back().start < tt) //at end
		{
			s->Finish();
			if (!s->calledComplete)
			{
				events.Enqueue(TimelineEvent(TimelineEventType::AnimationCompletion, this, s));
				s->calledComplete = true;
			}
			continue;
		}
		s->calledComplete = false;
		isDone = false;

		//get current and next frames
		size_t cfr = GetCurrentKeyframe(s);
		size_t nfr = (cfr + 1) % s->keyframes.size();

		s->Update(cfr, nfr, KeyframePercent(s->keyframes[cfr], s->keyframes[nfr]));

		//moved to the next keyframe
		if (s->lastKeyframe != cfr)
		{
			events.Enqueue(TimelineEvent(TimelineEventType::AnimationKeyframe, this, s));
			s->lastKeyframe = cfr;
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
	for (size_t i = 0; i < sequences.size(); i++)
	{
		if (sequences[i]->keyframes.size() > 0 && sequences[i]->keyframes.back().start > max)
			max = sequences[i]->keyframes.back().start;
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