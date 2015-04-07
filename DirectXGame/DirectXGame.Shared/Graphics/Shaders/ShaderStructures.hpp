#pragma once

#include "Pch.hpp"
#include "Common/SimpleMath.hpp"
#include "Graphics/ConstantBuffer.hpp"

using namespace DirectX::SimpleMath;

#define MAX_JOINTS 128 //The maximum allowed number of joints per shader

typedef ComPtr<ID3D11InputLayout> InputLayout;

//A basic world view projection matrix
struct WVPBufferDef : public ConstantBufferDef
{
	Matrix wvp; //World * View * Projection matrix

	WVPBufferDef() { }
	WVPBufferDef(const Matrix& WorldViewProjection) : wvp(WorldViewProjection) { }
	WVPBufferDef(const Matrix& World, const Matrix& View, const Matrix& Projection) : wvp((Projection * View) * World) { }
};

//A constant buffer with basic information specific to a single object
struct ObjectBufferDef : public ConstantBufferDef
{
	Matrix world;
	Matrix inverseTransposeWorld;
	Matrix worldViewProjection;

	//Calculate the inverse transpose world and world view projection matrix. Assumes ViewT and ProjectionT to be tranposed
	inline void Calc(const Matrix& ViewT, const Matrix& ProjectionT)
	{
		inverseTransposeWorld = world.Invert().Transpose();
		worldViewProjection = (ProjectionT * ViewT) * world;
	}
};