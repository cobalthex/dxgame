#pragma once

#include "Pch.hpp"
#include "Common/SimpleMath.hpp"
#include "ConstantBuffer.hpp"

using namespace DirectX::SimpleMath;

#define MAX_LIGHTS 8

//The available light types
enum class LightType
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

struct LightConstantBufferDef : public ConstantBufferDef
{
	Vector4 eyePosition;
	Color globalAmbience;

	Light lights[MAX_LIGHTS];
};