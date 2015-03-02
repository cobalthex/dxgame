#pragma once

#include "Pch.hpp"
#include "Common/SimpleMath.hpp"
#include "Graphics/ConstantBuffer.hpp"

using namespace DirectX::SimpleMath;

#define MAX_JOINTS 128 //The maximum allowed number of joints per shader

//A basic world view projection matrix
struct WVPConstantBufferDef : public ConstantBufferDef
{
	Matrix wvp; //World * View * Projection matrix

	WVPConstantBufferDef() : wvp() { }
	WVPConstantBufferDef(const Matrix& WorldViewProjection) : wvp(WorldViewProjection) { }
	WVPConstantBufferDef(const Matrix& World, const Matrix& View, const Matrix& Projection) : wvp((Projection * View) * World) { }
};

//A constant buffer with basic information specific to a single object
struct ObjectConstantBufferDef : public ConstantBufferDef
{
	Matrix world;
	Matrix inverseTransposeWorld;
	Matrix worldViewProjection;

	Matrix joints[MAX_JOINTS];

	//Calculate the inverse transpose world and world view projection matrix. Assumes ViewT and ProjectionT to be tranposed
	inline void Calc(const Matrix& ViewT, const Matrix& ProjectionT)
	{
		inverseTransposeWorld = world.Invert().Transpose();
		worldViewProjection = (ProjectionT * ViewT) * world;
	}
};

namespace VertexTypes
{
	//Used to send per-vertex data to the vertex shader
	struct VertexPositionColor
	{
		Vector3 position;
		Vector3 color;

		static const D3D11_INPUT_ELEMENT_DESC ElementDesc[];
		static const unsigned ElementCount;
	};

	//A simple vertex with only position and texture coordinates Useful for 2D
	struct VertexPositionTexture
	{
		Vector3 position;
		Vector2 texCoord;

		static const D3D11_INPUT_ELEMENT_DESC ElementDesc[];
		static const unsigned ElementCount;
	};

	//A single vertex that contains a full set of properties for skinned models
	struct VertexSkinned
	{
		Vector3 position;
		Vector3 normal;
		Vector4 tangent;
		Vector2 texCoord;
		DirectX::PackedVector::XMUBYTEN4 color;
		DirectX::PackedVector::XMUBYTE4 indices;
		DirectX::PackedVector::XMUBYTEN4 weights;

		//The element description of this vertex
		static const D3D11_INPUT_ELEMENT_DESC ElementDesc[];
		static const unsigned ElementCount;
	};
}