#pragma once

#include "Math.hpp"

using DirectX::Math::Vector2;
using DirectX::Math::Vector3;

#pragma push_macro("min")
#pragma push_macro("max")
#undef min
#undef max

struct Box2
{
	Vector2 min, max;

	Box2() { }
	Box2(const Box2& B) : min(B.min), max(B.max) { }
	Box2(const float Min[2], const float Max[2]) : min(Min == nullptr ? Vector2::Zero : Vector2(Min)), max(Max == nullptr ? Vector2::Zero : Vector2(Max)) { }
	Box2(const Vector2& Min, const Vector2& Max) : min(Min), max(Max) { }

	inline Box2& operator = (const Box2& B)
	{
		if (&B != this)
		{
			min = B.min;
			max = B.max;
		}
		return *this;
	}

	inline Box2 operator + (const Box2& B)
	{
		return Box2(min + B.min, max + B.max);
	}
};

//A 3D Box
struct Box3
{
	Vector3 min, max;

	Box3() { }
	Box3(const Box3& B) : min(B.min), max(B.max) { }
	Box3(const float Min[3], const float Max[3]) : min(Min == nullptr ? Vector3::Zero : Vector3(Min)), max(Max == nullptr ? Vector3::Zero : Vector3(Max)) { }
	Box3(const Vector3& Min, const Vector3& Max) : min(Min), max(Max) { }

	inline Box3& operator = (const Box3& B)
	{
		if (&B != this)
		{
			min = B.min;
			max = B.max;
		}
		return *this;
	}

	inline Box3 operator + (const Box3& B)
	{
		return Box3(min + B.min, max + B.max);
	}
};

#pragma pop_macro("min")
#pragma pop_macro("max")