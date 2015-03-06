#include "Pch.hpp"
#include "util.hpp"
#include "IqmLoader.hpp"
#include "Common/SimpleMath.hpp"
#include "Graphics/ShaderStructures.hpp" //contains vertex types

using namespace DirectX::SimpleMath;

//Todo: maybe move to memory mapped files

//Temporary storage for loading IQM data
struct IqmTemp
{
	const char* file = nullptr; //the name of the file that was loaded (Only valid for the duration of Load()
	Iqm::Header header;

	uint8_t* buffer = nullptr; //The file read into memory. Most of the pointers in this temp are directly mapped here

	Iqm::Mesh* meshes = nullptr;
	Iqm::Vertex* vertices = nullptr;
	Iqm::Normal* normals = nullptr;
	Iqm::Tangent* tangents = nullptr;
	Iqm::TexCoord* texCoords = nullptr;
	Iqm::Color* colors = nullptr;
	Iqm::BlendIndex* blendIndices = nullptr;
	Iqm::BlendWeight* blendWeights = nullptr;

	Iqm::Triangle* tris = nullptr;
	Iqm::Adjacency* adjacencies = nullptr;
	Iqm::Joint* joints = nullptr;
	Iqm::Pose* poses = nullptr;
	Iqm::Anim* anims = nullptr;
	Iqm::Bounds* bounds = nullptr;
	//Matrix* frames = nullptr; //animation frames

	::Pose* genPoses; //A collection of generated poses from the Iqm poses (all of these poses will be assigned to the model)
	::Joint* genJoints; //A collection of joints in the mesh (all of these joints will be assigned to the model)

	//Matrix* baseFrames = nullptr;
	//Matrix* inverseBaseFrames = nullptr;

	char* texts = nullptr; //all of the texts
	char* comments = nullptr; //all of the comments

	Iqm::Extension* extensions = nullptr;
};

bool LoadMeshes(IqmTemp& Temp); //Load the meshes (and materials) from the buffer
bool LoadAnimations(IqmTemp& Temp); //Load the animations from the buffer

void CleanupTemp(IqmTemp& Temp)
{
	delete[] Temp.buffer;

	delete[] Temp.genPoses;
	delete[] Temp.genJoints;
}

bool Iqm::Load(const DX::DeviceResourcesPtr& DeviceResources, ContentCache& Cache, const std::string& Filename, __out ::Model& mdl)
{
	const char* fn = Filename.c_str();
	FILE* f = nullptr;
	auto err = fopen_s(&f, fn, "rb");

	if (f == nullptr && err != 0)
	{
		//OutputDebugString((std::wstring(L"Error loading IQM: ") + _wcserror_s(err)).c_str());
		return false;
	}

	//Store temp data
	IqmTemp tmp;
	tmp.file = fn;

	//load header
	if (fread(&tmp.header, 1, sizeof(tmp.header), f) != sizeof(tmp.header) || memcmp(tmp.header.magic, IQM_MAGIC, sizeof(tmp.header.magic)))
		return false;

	LilSwap(&tmp.header.version, (sizeof(tmp.header) - sizeof(tmp.header.magic)) / sizeof(uint));

	if (tmp.header.version != IQM_VERSION)
		return false;
	
	if (tmp.header.fileSize > (16 << 20))
		return false; //sanity check... don't load files bigger than 16 MB

	tmp.buffer = new uchar[tmp.header.fileSize];
	
	//read the file into memory
	if (fread(tmp.buffer + sizeof(tmp.header), 1, tmp.header.fileSize - sizeof(tmp.header), f) != tmp.header.fileSize - sizeof(tmp.header))
		return false;
	fclose(f);

	if (tmp.header.numMeshes > 0 && !LoadMeshes(tmp))
		return false;
	if (tmp.header.numAnims > 0 && !LoadAnimations(tmp))
		return false;

	//Create model from generated data

	std::vector<::ModelMesh> meshes; meshes.reserve(tmp.header.numMeshes);
	std::vector<::Model::VertexType> vertices;
	std::vector<unsigned> indices;
	for (unsigned i = 0; i < tmp.header.numMeshes; i++)
	{
		auto& m = tmp.meshes[i];

		indices.reserve(indices.size() + m.numTriangles * 3);
		vertices.reserve(vertices.size() + m.numVertices);

		//indices
		for (unsigned j = 0; j < m.numTriangles; j++)
		{
			//For independent vertex buffers, subtract m.firstVertex from each of the vertices of the triangle
			indices.push_back(tmp.tris[m.firstTriangle + j].vertex[0]);
			indices.push_back(tmp.tris[m.firstTriangle + j].vertex[1]);
			indices.push_back(tmp.tris[m.firstTriangle + j].vertex[2]);
		}

		//vertices
		for (unsigned j = 0; j < m.numVertices; j++)
		{
			::Model::VertexType v;

			auto& _v = tmp.vertices[m.firstVertex + j];
			auto& _n = tmp.normals[m.firstVertex + j];
			auto& _g = tmp.tangents[m.firstVertex + j];
			auto& _t = tmp.texCoords[m.firstVertex + j];
			auto& _c = tmp.colors[m.firstVertex + j];
			auto& _i = tmp.blendIndices[m.firstVertex + j];
			auto& _w = tmp.blendWeights[m.firstVertex + j];

			//correct orientation would be yzx for DX

			v.position = Vector3(_v.x, _v.y, _v.z);
			if (tmp.normals != nullptr)
				v.normal = Vector3(_n.x, _n.y, _n.z);
			if (tmp.tangents != nullptr)
				v.tangent = Vector4(_g.x, _g.y, _g.z, _g.w);
			if (tmp.texCoords != nullptr)
				v.texCoord = Vector2(_t.u, _t.v);
			if (tmp.colors != nullptr)
				v.color = DirectX::PackedVector::XMUBYTEN4(_c.r, _c.g, _c.b, 1);
			if (tmp.blendIndices != nullptr)
				v.indices = DirectX::PackedVector::XMUBYTE4(_i.a, _i.b, _i.c, _i.d);
			if (tmp.blendWeights != nullptr) 
				v.weights = DirectX::PackedVector::XMUBYTEN4(_w.a, _w.b, _w.c, _w.d);

			vertices.push_back(v);
		}

		//material
		std::string texFile = "Content/" + std::string(tmp.texts + m.material);
		Material mat;
		mat.texture = Cache.Add(texFile, std::make_shared<Texture2D>(DeviceResources, texFile));
		mat.useTexture = (mat.texture != nullptr);
		mat.emissive = ::Color(0, 0, 0, 1);
		mat.ambient = ::Color(1, 1, 1, 1);
		mat.diffuse = ::Color(1, 1, 1, 1);
		mat.specular = ::Color(1, 1, 1, 1);
		mat.specularPower = 1;

		meshes.emplace_back(m.firstVertex, m.numVertices, m.firstTriangle * 3, m.numTriangles * 3, mat);
	}

	std::map<std::string, ::SkinnedSequence> sequences;

	//create animations
	for (unsigned i = 0; i < tmp.header.numAnims; i++)
	{
		auto& a = tmp.anims[i];

		std::vector<::Pose> poses (tmp.genPoses + a.firstFrame, tmp.genPoses + a.firstFrame + a.numFrames);

		::SkinnedSequence s (poses);
		s.Keyframes().reserve(a.numFrames);

		unsigned millis = 16; //~60fps
		if (a.frameRate > 0)
			millis = (unsigned)(1000 / a.frameRate);

		//create frames
		for (unsigned i = 0; i < a.numFrames; i++)
			s.Append(Keyframe(TimePoint(TimeType(i * millis))));

		std::string pname = std::string(tmp.texts + a.name);
		sequences[pname] = s;
	}

	mdl = Model(DeviceResources, vertices, indices, PrimitiveTopology::List, meshes, std::vector<::Joint>(tmp.genJoints, tmp.genJoints + tmp.header.numJoints), sequences);
	if (tmp.header.numAnims > 0)
		mdl.pose = std::string(tmp.texts + tmp.anims[0].name);

	CleanupTemp(tmp);
	return true;
}

bool LoadMeshes(IqmTemp& Temp)
{
	//make sure in correct endian
	LilSwap((unsigned*)&Temp.buffer[Temp.header.offsetVertexArrays], Temp.header.numVertexArrays * sizeof(Iqm::VertexArray) / sizeof(unsigned));
	LilSwap((unsigned*)&Temp.buffer[Temp.header.offsetTriangles], Temp.header.numTriangles * sizeof(Iqm::Triangle) / sizeof(unsigned));
	LilSwap((unsigned*)&Temp.buffer[Temp.header.offsetMeshes], Temp.header.numMeshes * sizeof(Iqm::Mesh) / sizeof(unsigned));
	LilSwap((unsigned*)&Temp.buffer[Temp.header.offsetJoints], Temp.header.numJoints * sizeof(Iqm::Joint) / sizeof(unsigned));

	uchar* vindex = NULL, *vweight = NULL;
	Iqm::VertexArray* vas = (Iqm::VertexArray*)&Temp.buffer[Temp.header.offsetVertexArrays];
	for (unsigned i = 0; i < Temp.header.numVertexArrays; i++)
	{
		Iqm::VertexArray &va = vas[i];
		switch (va.type)
		{
		case Iqm::IQM_POSITION:
			//Make sure format is valid
			if (va.format != Iqm::IQM_FLOAT || va.size != 3)
				return false;

			Temp.vertices = (Iqm::Vertex*)&Temp.buffer[va.offset];
			LilSwap(Temp.vertices, 3 * Temp.header.numVertices);
			break;

		case Iqm::IQM_NORMAL:
			if (va.format != Iqm::IQM_FLOAT || va.size != 3)
				return false;

			Temp.normals = (Iqm::Normal*)&Temp.buffer[va.offset];
			LilSwap(Temp.normals, 3 * Temp.header.numVertices);
			break;

		case Iqm::IQM_TANGENT:
			if (va.format != Iqm::IQM_FLOAT || va.size != 4)
				return false;

			Temp.tangents = (Iqm::Tangent*)&Temp.buffer[va.offset];
			LilSwap(Temp.tangents, 4 * Temp.header.numVertices);
			break;

		case Iqm::IQM_TEXCOORD:
			if (va.format != Iqm::IQM_FLOAT || va.size != 2)
				return false;

			Temp.texCoords = (Iqm::TexCoord*)&Temp.buffer[va.offset];
			LilSwap(Temp.texCoords, 2 * Temp.header.numVertices);
			break;

		case Iqm::IQM_COLOR:
			if (va.format != Iqm::IQM_UBYTE || va.size != 4)
				return false;

			Temp.colors = (Iqm::Color*)&Temp.buffer[va.offset];
			LilSwap(Temp.colors, 4 * Temp.header.numVertices);
			break;

		case Iqm::IQM_BLENDINDEXES:
			if (va.format != Iqm::IQM_UBYTE || va.size != 4)
				return false;

			Temp.blendIndices = (Iqm::BlendIndex*)&Temp.buffer[va.offset];
			break;

		case Iqm::IQM_BLENDWEIGHTS:
			if (va.format != Iqm::IQM_UBYTE || va.size != 4)
				return false;

			Temp.blendWeights = (Iqm::BlendWeight*)&Temp.buffer[va.offset];
			break;
		}
	}

	if (Temp.vertices == nullptr)
		return false;

	Temp.meshes = (Iqm::Mesh*)&Temp.buffer[Temp.header.offsetMeshes];
	Temp.tris = (Iqm::Triangle*)&Temp.buffer[Temp.header.offsetTriangles];
	Temp.joints = (Iqm::Joint*)&Temp.buffer[Temp.header.offsetJoints];

	if (Temp.header.offsetAdjacency > 0)
		Temp.adjacencies = (Iqm::Adjacency*)&Temp.buffer[Temp.header.offsetAdjacency];

	Temp.texts = Temp.header.offsetTexts > 0 ? (char*)&Temp.buffer[Temp.header.offsetTexts] : "";
	Temp.comments = Temp.header.offsetComments > 0 ? (char*)&Temp.buffer[Temp.header.offsetComments] : "";

	//load joints
	
	Temp.genJoints = new Joint[Temp.header.numJoints];
	for (unsigned i = 0; i < Temp.header.numJoints; i++)
	{
		auto& j = Temp.joints[i];

		Temp.genJoints[i].index = i;
		Temp.genJoints[i].parent = j.parent;
		Temp.genJoints[i].name = std::string(Temp.texts + j.name);

		auto q = Quaternion(j.rotate); q.Normalize();
		Matrix scale = Matrix::CreateScale(Vector3(j.scale));
		Matrix rotation = Matrix::CreateFromQuaternion(q);
		Matrix translation = Matrix::CreateTranslation(Vector3(j.translate));

		Temp.genJoints[i].transform = (scale * rotation) * translation;
		Temp.genJoints[i].inverseTransform = Temp.genJoints[i].transform.Invert();

		if (j.parent >= 0)
		{
			Temp.genJoints[i].transform *= Temp.genJoints[j.parent].transform;
			Temp.genJoints[i].inverseTransform = Temp.genJoints[j.parent].inverseTransform * Temp.genJoints[i].inverseTransform;
		}
	}
	
	return true;
}
bool LoadAnimations(IqmTemp& Temp)
{
	LilSwap((unsigned*)&Temp.buffer[Temp.header.offsetPoses], Temp.header.numPoses * sizeof(Iqm::Pose) / sizeof(unsigned));
	LilSwap((unsigned*)&Temp.buffer[Temp.header.offsetAnims], Temp.header.numAnims * sizeof(Iqm::Anim) / sizeof(unsigned));
	LilSwap((unsigned*)&Temp.buffer[Temp.header.offsetFrames], Temp.header.numFrames * Temp.header.numFrameChannels);

	Temp.anims = (Iqm::Anim*)&Temp.buffer[Temp.header.offsetAnims];
	Temp.poses = (Iqm::Pose*)&Temp.buffer[Temp.header.offsetPoses];
	//Temp.frames = new Matrix[Temp.header.numFrames * Temp.header.numPoses];
	unsigned short* frameData = (unsigned short*)&Temp.buffer[Temp.header.offsetFrames];

	Temp.genPoses = new ::Pose[Temp.header.numFrames];

	for (unsigned i = 0; i < Temp.header.numFrames; i++)
	{
		auto& pose = Temp.genPoses[i];
		pose = Pose(Temp.header.numPoses);

		for (unsigned j = 0; j < Temp.header.numPoses; j++)
		{
			auto p = Temp.poses[j];

			pose.translations[j].x = p.channelOffset[0]; if (p.mask & 0x01) pose.translations[j].x += *frameData++ * p.channelScale[0];
			pose.translations[j].y = p.channelOffset[1]; if (p.mask & 0x02) pose.translations[j].y += *frameData++ * p.channelScale[1];
			pose.translations[j].z = p.channelOffset[2]; if (p.mask & 0x04) pose.translations[j].z += *frameData++ * p.channelScale[2];
			
			pose.rotations[j].x = p.channelOffset[3]; if (p.mask & 0x08) pose.rotations[j].x += *frameData++ * p.channelScale[3];
			pose.rotations[j].y = p.channelOffset[4]; if (p.mask & 0x10) pose.rotations[j].y += *frameData++ * p.channelScale[4];
			pose.rotations[j].z = p.channelOffset[5]; if (p.mask & 0x20) pose.rotations[j].z += *frameData++ * p.channelScale[5];
			pose.rotations[j].w = p.channelOffset[6]; if (p.mask & 0x40) pose.rotations[j].w += *frameData++ * p.channelScale[6];
			pose.rotations[j].Normalize();
			
			pose.scales[j].x = p.channelOffset[7]; if (p.mask & 0x080) pose.scales[j].x += *frameData++ * p.channelScale[7];
			pose.scales[j].y = p.channelOffset[8]; if (p.mask & 0x100) pose.scales[j].y += *frameData++ * p.channelScale[8];
			pose.scales[j].z = p.channelOffset[9]; if (p.mask & 0x200) pose.scales[j].z += *frameData++ * p.channelScale[9];
		}
	}

	return true;
}