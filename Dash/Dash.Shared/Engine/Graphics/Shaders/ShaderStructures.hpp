#pragma once

#include "Pch.hpp"
#include "Engine/Common/Math.hpp"
#include "Engine/Graphics/ConstantBuffer.hpp"

using namespace Math;
using namespace DirectX::PackedVector;

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

//All of the available vertex formats
namespace Vertexes
{
	struct StaticVertex
	{
		Vector3 position;
		Vector3 normal;
		Vector4 tangent;
		Vector2 texCoord;
		XMUBYTEN4 color;

		StaticVertex() = default;
		StaticVertex(
			const Vector3& Position,
			const Vector3& Normal,
			const Vector4& Tangent,
			const Vector2& TexCoord,
			const Color& Color
		) :
		position(Position),
		normal(Normal),
		tangent(Tangent),
		texCoord(TexCoord),
		color(Color) { }

		//The element description of this vertex
		static const D3D11_INPUT_ELEMENT_DESC ElementDesc[];
		static const unsigned ElementCount;
	};

	struct SkinnedVertex
	{
		Vector3 position;
		Vector3 normal;
		Vector4 tangent;
		Vector2 texCoord;
		XMUBYTEN4 color;
		XMUBYTE4 blIndices;
		XMUBYTEN4 blWeights;

		SkinnedVertex() = default;
		SkinnedVertex(
			const Vector3& Position,
			const Vector3& Normal,
			const Vector4& Tangent,
			const Vector2& TexCoord,
			const Color& Color,
			const Vector4& BlendIndices,
			const Vector4& BlendWeights
		) :
		position(Position),
		normal(Normal),
		tangent(Tangent),
		texCoord(TexCoord),
		color(Color),
		blIndices(BlendIndices),
		blWeights(BlendWeights) { }

		static const D3D11_INPUT_ELEMENT_DESC ElementDesc[];
		static const unsigned ElementCount;
	};

	struct PositionTextureVertex
	{
		Vector3 position;
		Vector2 texCoord;

		PositionTextureVertex() = default;
		PositionTextureVertex(const Vector3& Position, const Vector2& TexCoord) : position(Position), texCoord(TexCoord) { }

		static const D3D11_INPUT_ELEMENT_DESC ElementDesc[];
		static const unsigned ElementCount;
	};
};