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
	WVPConstantBufferDef(const Matrix& World, const Matrix& View, const Matrix& Projection) : wvp((World * View) * Projection) { }
};

//A constant buffer with basic information specific to a single object
struct ObjectConstantBufferDef : public ConstantBufferDef
{
	Matrix world;
	Matrix inverseTransposeWorld;
	Matrix worldViewProjection;

	DirectX::XMFLOAT4X3A joints[MAX_JOINTS];

	//Calculate the inverse transpose world and world view projection matrices
	inline void Calc(const Matrix& View, const Matrix& Projection)
	{
		inverseTransposeWorld = world.Invert().Transpose();
		worldViewProjection = (Projection * View) * world;
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

//A few helper functions for when creating vertices
namespace DX
{
	//Pack a list of unsigned integers into a single 32 bit unsigned value (assumes little endian)
	inline uint32_t PackInts(uint32_t A, uint32_t B, uint32_t C, uint32_t D)
	{
#ifdef IS_BIG_ENDIAN
		return (((D & 0xff) << 24) + ((C & 0xff) << 16) + ((B & 0xff) << 8) + (A & 0xff));
#else
		return (((A & 0xff) << 24) + ((B & 0xff) << 16) + ((C & 0xff) << 8) + (D & 0xff));
#endif
	}
	inline uint32_t PackInts(const DirectX::XMUINT4& Ints) { return PackInts(Ints.x, Ints.y, Ints.z, Ints.w); }
}