#pragma once

#include "Pch.hpp"

enum class CurveContinuity
{
	Smooth,
	Step
};
enum class CurveTangent
{
	Flat,
	Linear,
	Smooth
};
enum class CurveLoopType
{
	Constant,	//The Curve will evaluate to its first key for positions before the first point in the Curve and to the last key for positions after the last point
	Cycle,		//Positions specified past the ends of the curve will wrap around to the opposite side of the Curve
	CycleOffset,//Positions specified past the ends of the curve will wrap around to the opposite side of the Curve. The value will be offset by the difference between the values of the first and last CurveKey multiplied by the number of times the position wraps around. If the position is before the first point in the Curve, the difference will be subtracted from its value; otherwise, the difference will be added
	Oscillate,	//Linear interpolation will be performed to determine the value
	Linear		//Positions specified past the ends of the Curve act as an offset from the same side of the Curve toward the opposite side
};

//A single point on a curve
template <typename T = float>
class CurveKey
{
public:
	CurveContinuity continuity;

	float position; //percentage (0 to 1) of this location's position in the curve
	float tangentIn; //the tangent approaching this point from the previous point
	float tangentOut; //the tangent leaving this point for the next
	
	T value; //the value of this point
};

//A single dimensional curve
class Curve
{
public:
	using ValueType = float;
	using KeyType = CurveKey<ValueType>;

	std::vector<KeyType> keys; //All of the keys in this curve

	CurveLoopType preLoop; //How to handle values smaller than the beginning
	CurveLoopType postLoop; //How to handle values greater than the end

	void ComputeTangent(size_t KeyIndex, CurveTangent TangentType); //Compute a tangent for a single key
	void ComputeTangents(CurveTangent TangentType); //Compute all tangents in the curve

	ValueType Evaluate(float Position);
	ValueType GetCurvePosition(float Position);

	int GetNumberOfCycles(float Position);
};

class Curve3d
{
public:
	Curve xCurve;
	Curve yCurve;
	Curve zCurve;
};