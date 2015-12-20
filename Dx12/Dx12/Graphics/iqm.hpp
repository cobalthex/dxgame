#pragma once

#define IQMAGIC "INTERQUAKEMODEL"
#define IQVERSION 2

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
		IQMPOSITION = 0,		//float, 3
		IQMTEXCOORD = 1,		//float, 2
		IQMNORMAL = 2,			//float, 3
		IQMTANGENT = 3,			//float, 4
		IQMBLENDINDEXES = 4,	//ubyte, 4
		IQMBLENDWEIGHTS = 5,	//ubyte, 4
		IQMCOLOR = 6,			//ubyte, 4
		IQMCUSTOM = 0x10
	};

	enum Formats
	{
		IQMBYTE = 0,
		IQMUBYTE = 1,
		IQMSHORT = 2,
		IQMUSHORT = 3,
		IQMINT = 4,
		IQMUINT = 5,
		IQMHALF = 6,
		IQMFLOAT = 7,
		IQMDOUBLE = 8
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
		IQLOOP = 1 << 0
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
		unsigned int offsetExtensions; //pointer to next extension
	};
}