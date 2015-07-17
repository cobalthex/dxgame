#pragma once

#include "Pch.hpp"
#include "Engine/Common/Math.hpp"
#include "ConstantBuffer.hpp"

using namespace Math;

#define MAX_LIGHTS 4

//The available light types
enum class LightType : unsigned
{
	Directional = 0, //Light that emits in a direction but has no position 
	Point = 1, //Emits light around a point in every direction
	Spot = 2 //Emits light from a point in a specific direction
};

CBUFFER_ALIGN struct Light
{
	Vector4 position;
	Vector4 direction;
	Color color;

	float spotAngle;
	float constantAttenuation;
	float linearAttenuation;
	float quadraticAttenuation;

	LightType type;
	bool isEnabled = false;
};

struct LightBufferDef : public ConstantBufferDef
{
	Vector4 eyePosition;
	Color globalAmbience;
	Color shadowColor;

	Light lights[MAX_LIGHTS];
};