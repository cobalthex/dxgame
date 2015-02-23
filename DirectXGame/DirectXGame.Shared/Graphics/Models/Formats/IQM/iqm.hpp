#pragma once

#define IQM_MAGIC "INTERQUAKEMODEL"
#define IQM_VERSION 2

namespace Iqm
{
	//The IQM Header
	struct Header
	{
		char magic[16];
		unsigned int version;
		unsigned int fileSize;
		unsigned int flags;
		//file position offsets and counts
		unsigned int numTexts, offsetTexts;
		unsigned int numMeshes, offsetMeshes;
		unsigned int numVertexArrays, numVertices, offsetVertexArrays;
		unsigned int numTriangles, offsetTriangles, offsetAdjacency;
		unsigned int numJoints, offsetJoints;
		unsigned int numPoses, offsetPoses;
		unsigned int numAnims, offsetAnims;
		unsigned int numFrames, numFrameChannels, offsetFrames, offsetBounds;
		unsigned int numComments, offsetComments;
		unsigned int numExtensions, offsetExtensions;
	};

	struct Mesh
	{
		unsigned int name;
		unsigned int material;
		unsigned int firstVertex, numVertices;
		unsigned int firstTriangle, numTriangles;
	};

	enum VertexArrayTypes
	{
		IQM_POSITION = 0,		// float, 3
		IQM_TEXCOORD = 1,		// float, 2
		IQM_NORMAL = 2,			// float, 3
		IQM_TANGENT = 3,		// float, 4
		IQM_BLENDINDEXES = 4,	// ubyte, 4
		IQM_BLENDWEIGHTS = 5,	// ubyte, 4
		IQM_COLOR = 6,			// ubyte, 4
		IQM_CUSTOM = 0x10
	};

	enum Formats
	{
		IQM_BYTE = 0,
		IQM_UBYTE = 1,
		IQM_SHORT = 2,
		IQM_USHORT = 3,
		IQM_INT = 4,
		IQM_UINT = 5,
		IQM_HALF = 6,
		IQM_FLOAT = 7,
		IQM_DOUBLE = 8
	};

	struct Vertex
	{
		float x, y, z;
	};
	typedef Vertex Normal;
	struct Tangent
	{
		float x, y, z, w;
	};
	struct TexCoord
	{
		float u, v;
	};
	struct Color
	{
		uint8_t r, g, b, a;
	};
	struct BlendIndex
	{
		uint8_t a, b, c, d;
	};
	typedef BlendIndex BlendWeight;

	struct Triangle
	{
		unsigned int vertex[3];
	};

	struct Adjacency
	{
		unsigned int triangle[3];
	};

	struct Jointv1
	{
		unsigned int name;
		int parent;
		float translate[3], rotate[3], scale[3];
	};

	struct Joint
	{
		unsigned int name;
		int parent;
		float translate[3], rotate[4], scale[3];
	};

	struct Pose
	{
		int parent;
		unsigned int mask;
		float channelOffset[10];
		float channelScale[10];
	};

	struct Anim
	{
		unsigned int name;
		unsigned int firstFrame, numFrames;
		float frameRate;
		unsigned int flags;
	};

	enum Loop
	{
		IQM_LOOP = 1 << 0
	};

	struct VertexArray
	{
		unsigned int type;
		unsigned int flags;
		unsigned int format;
		unsigned int size;
		unsigned int offset;
	};

	struct Bounds
	{
		float bbMin[3], bbAx[3];
		float xyRadius, radius;
	};

	struct Extension
	{
		unsigned int name;
		unsigned int numData, offsetData;
		unsigned int offsetExtensions; // pointer to next extension
	};
}