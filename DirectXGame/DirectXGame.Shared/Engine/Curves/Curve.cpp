#include "Pch.hpp"
#include "Curve.hpp"

void Curve::ComputeTangent(size_t KeyIndex, CurveTangent TangentType)
{

}
void Curve::ComputeTangents(CurveTangent TangentType)
{

}

Curve::ValueType Curve::Evaluate(float Position)
{
	KeyType& first = keys[0];
	KeyType& last = keys.back();

	int cycle = 0;
	float virtualPos = 0;

	if (Position < first.position)
	{
		switch (preLoop)
		{
		case CurveLoopType::Constant:
			//constant
			return first.value;

		case CurveLoopType::Linear:
			// linear y = a*x +b with a tangeant of last point
			return first.value - first.tangentIn * (first.position - Position);

		case CurveLoopType::Cycle:
			//start -> end / start -> end
			cycle = GetNumberOfCycles(Position);
			virtualPos = Position - (cycle * (last.position - first.position));
			return GetCurvePosition(virtualPos);

		case CurveLoopType::CycleOffset:
			//make the curve continue (with no step) so must up the curve each cycle of delta(value)
			cycle = GetNumberOfCycles(Position);
			virtualPos = Position - (cycle * (last.position - first.position));
			return (GetCurvePosition(virtualPos) + cycle * (last.value - first.value));

		case CurveLoopType::Oscillate:
			//go back on curve from end and target start 
			// start-> end / end -> start
			cycle = GetNumberOfCycles(Position);
			if (fmod(cycle, 2.f) == 0) //if pair
				virtualPos = Position - (cycle * (last.position - first.position));
			else
				virtualPos = last.position - Position + first.position + (cycle * (last.position - first.position));
			return GetCurvePosition(virtualPos);
		}
	}
	else if (Position > last.position)
	{
		switch (postLoop)
		{
		case CurveLoopType::Constant:
			//constant
			return last.value;

		case CurveLoopType::Linear:
			// linear y = a*x +b with a tangeant of last point
			return last.value + first.tangentOut * (Position - last.position);

		case CurveLoopType::Cycle:
			//start -> end / start -> end
			cycle = GetNumberOfCycles(Position);
			virtualPos = Position - (cycle * (last.position - first.position));
			return GetCurvePosition(virtualPos);

		case CurveLoopType::CycleOffset:
			//make the curve continue (with no step) so must up the curve each cycle of delta(value)
			cycle = GetNumberOfCycles(Position);
			virtualPos = Position - (cycle * (last.position - first.position));
			return (GetCurvePosition(virtualPos) + cycle * (last.value - first.value));

		case CurveLoopType::Oscillate:
			//go back on curve from end and target start 
			// start-> end / end -> start
			cycle = GetNumberOfCycles(Position);
			virtualPos = Position - (cycle * (last.position - first.position));
			if (fmod(cycle, 2) == 0)//if pair
				virtualPos = Position - (cycle * (last.position - first.position));
			else
				virtualPos = last.position - Position + first.position + (cycle * (last.position - first.position));
			return GetCurvePosition(virtualPos);
		}
	}

	//in curve
	return GetCurvePosition(Position);
}
Curve::ValueType Curve::GetCurvePosition(float Position)
{
	if (keys.size() < 1)
		return 0;

	//only for position in curve
	KeyType* prev = &keys[0];
	KeyType* next = nullptr;
	for (int i = 1; i < keys.size(); i++)
	{
		next = &keys[i];
		if (next->position >= Position)
		{
			if (prev->continuity == CurveContinuity::Step)
			{
				if (Position >= 1)
				{
					return next->value;
				}
				return prev->value;
			}
			float t = (Position - prev->position) / (next->position - prev->position); //to have t in [0,1]
			float ts = t * t;
			float tss = ts * t;
			//http://en.wikipedia.org/wiki/Cubic_Hermite_spline
			//P(t) = (2*t^3 - 3t^2 + 1)*P0 + (t^3 - 2t^2 + t)m0 + (-2t^3 + 3t^2)P1 + (t^3-t^2)m1
			//with P0.value = prev.value , m0 = prev.tangentOut, P1= next.value, m1 = next.TangentIn
			return (2 * tss - 3 * ts + 1) * prev->value + (tss - 2 * ts + t) * prev->tangentOut + (3 * ts - 2 * tss) * next->value + (tss - ts) * next->tangentIn;
		}
		prev = next;
	}
	return 0;
}

int Curve::GetNumberOfCycles(float Position)
{
	if (keys.size() < 1)
		return 0;

	float cycle = (Position - keys[0].position) / (keys.back().position - keys[0].position);
	if (cycle < 0)
		cycle--;

	return (int)cycle;
}
