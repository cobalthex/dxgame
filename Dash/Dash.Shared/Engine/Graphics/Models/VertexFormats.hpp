#pragma once

#include "Pch.hpp"
#include "Engine/Common/Math.hpp"

using namespace Math;
using namespace DirectX::PackedVector;

//All of the possible vertex formats
enum class VertexFormats
{
	Static,
	Skinned,
	PositionTexture,
	PositionColor
};

//All of the available vertex formats
namespace Vertexes
{
	template <VertexFormats Fmt>
	struct Vertex
	{
		static inline VertexFormats VertexFormat() { return Fmt; } //Get this type of vertex

		//The element description of this vertex
		static const D3D11_INPUT_ELEMENT_DESC ElementDesc[];
		static const unsigned ElementCount;

	protected:
		Vertex() { }
	};

	template <>
	struct Vertex<VertexFormats::Static>
	{
		Vector3 position;
		Vector3 normal;
		Vector4 tangent;
		Vector2 texCoord;
		XMUBYTEN4 color;

		Vertex() = default;
		Vertex(
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
	using StaticVertex = Vertex<VertexFormats::Static>;

	template <>
	struct Vertex<VertexFormats::Skinned>
	{
		Vector3 position;
		Vector3 normal;
		Vector4 tangent;
		Vector2 texCoord;
		XMUBYTEN4 color;
		XMUBYTE4 blIndices;
		XMUBYTEN4 blWeights;

		Vertex() = default;
		Vertex(
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

		//The element description of this vertex
		static const D3D11_INPUT_ELEMENT_DESC ElementDesc[];
		static const unsigned ElementCount;
	};
	using SkinnedVertex = Vertex<VertexFormats::Skinned>;

	template <>
	struct Vertex<VertexFormats::PositionTexture>
	{
		Vector3 position;
		Vector2 texCoord;

		Vertex() = default;
		Vertex(const Vector3& Position, const Vector2& TexCoord) : position(Position), texCoord(TexCoord) { }

		static const D3D11_INPUT_ELEMENT_DESC ElementDesc[];
		static const unsigned ElementCount;
	};
	using PositionTextureVertex = Vertex<VertexFormats::PositionTexture>;

	template <>
	struct Vertex<VertexFormats::PositionColor>
	{
		Vector3 position;
		Color color;

		Vertex() = default;
		Vertex(const Vector3& Position, const Color& Color) : position(Position), color(Color) { }

		static const D3D11_INPUT_ELEMENT_DESC ElementDesc[];
		static const unsigned ElementCount;
	};
	using PositionColorVertex = Vertex<VertexFormats::PositionColor>;
};