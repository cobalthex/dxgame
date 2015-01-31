#include "Pch.hpp"
#include "util.hpp"
#include "IqmLoader.hpp"
#include "Common/SimpleMath.hpp"
#include "Graphics/VertexTypes.hpp" //contains vertex types

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
	uint16_t* frames; //animation frames

	char* texts = nullptr; //all of the texts
	char* comments = nullptr; //all of the comments

	Iqm::Extension* extensions = nullptr;
};

bool LoadMeshes(IqmTemp& Temp); //Load the meshes (and materials) from the buffer
bool LoadAnimations(IqmTemp& Temp); //Load the animations from the buffer

void CleanupTemp(IqmTemp& Temp)
{
	delete[] Temp.buffer;
	Temp.buffer = 0;
	Temp.file = 0;
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
		return false; // sanity check... don't load files bigger than 16 MB

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

	std::vector<::Mesh> meshes;
	std::vector<::Bone> bones;
	std::vector<::Model::VertexType> vertices;
	std::vector<unsigned> indices;
	for (unsigned i = 0; i < tmp.header.numMeshes; i++)
	{
		auto& m = tmp.meshes[i];

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

			v.position = Vector3(_v.y, _v.z, _v.x); //correct orientation
			if (tmp.normals != nullptr)
				v.normal = Vector3(_n.x, _n.y, _n.z);
			if (tmp.tangents != nullptr)
				v.tangent = Vector4(_g.x, _g.y, _g.z, _g.w);
			if (tmp.texCoords != nullptr)
				v.texCoord = Vector2(_t.u, _t.v);
			if (tmp.colors != nullptr)
				v.color = DirectXGame::PackVector(_c.r, _c.g, _c.b, 1);
			if (tmp.blendIndices != nullptr)
				v.indices = DirectX::XMUINT4(_i.a, _i.b, _i.c, _i.d);
			if (tmp.blendWeights != nullptr)
				v.weights = DirectXGame::PackVector(_w.a, _w.b, _w.c, _w.d);

			vertices.push_back(v);
		}

		//material
		std::string texFile = "Content/" + std::string(tmp.texts + m.material);
		Material mat;
		mat.texture = Cache.CreateTexture2D(texFile, DeviceResources, texFile);
		mat.emissive = ::Color(0, 0, 0, 1);
		mat.ambient = ::Color(1, 1, 1, 1);
		mat.diffuse = ::Color(1, 1, 1, 1);
		mat.specular = ::Color(1, 1, 1, 1);
		mat.specularPower = 128;

		meshes.emplace_back(m.firstVertex, m.numVertices, m.firstTriangle * 3, m.numTriangles * 3, mat);
	}

	mdl = Model(DeviceResources, PrimitiveTopology::List, vertices, indices, meshes, bones);

	CleanupTemp(tmp);
	return true;
}

bool LoadMeshes(IqmTemp& Temp)
{
	//make sure in correct endian
	LilSwap((uint*)&Temp.buffer[Temp.header.ofsVertexArrays], Temp.header.numVertexArrays* sizeof(Iqm::VertexArray) / sizeof(uint));
	LilSwap((uint*)&Temp.buffer[Temp.header.ofsTriangles], Temp.header.numTriangles* sizeof(Iqm::Triangle) / sizeof(uint));
	LilSwap((uint*)&Temp.buffer[Temp.header.ofsMeshes], Temp.header.numMeshes* sizeof(Iqm::Mesh) / sizeof(uint));
	LilSwap((uint*)&Temp.buffer[Temp.header.ofsJoints], Temp.header.numJoints* sizeof(Iqm::Joint) / sizeof(uint));

	uchar* vindex = NULL, *vweight = NULL;
	Iqm::VertexArray* vas = (Iqm::VertexArray*)&Temp.buffer[Temp.header.ofsVertexArrays];
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

	Temp.meshes = (Iqm::Mesh*)&Temp.buffer[Temp.header.ofsMeshes];
	Temp.tris = (Iqm::Triangle*)&Temp.buffer[Temp.header.ofsTriangles];
	Temp.joints = (Iqm::Joint*)&Temp.buffer[Temp.header.ofsJoints];

	if (Temp.header.ofsAdjacency > 0)
		Temp.adjacencies = (Iqm::Adjacency*)&Temp.buffer[Temp.header.ofsAdjacency];

	Temp.texts = Temp.header.ofsText > 0 ? (char*)&Temp.buffer[Temp.header.ofsText] : "";
	Temp.comments = Temp.header.ofsComment > 0 ? (char*)&Temp.buffer[Temp.header.ofsComment] : "";

	//load joints
	auto baseFrame = new Matrix[Temp.header.numJoints];
	auto inverseBaseFrame = new Matrix[Temp.header.numJoints];
	for (unsigned i = 0; i < Temp.header.numJoints; i++)
	{
		auto& j = Temp.joints[i];

		auto q = Quaternion(j.rotate); q.Normalize();
		baseFrame[i] = Matrix((Vector3)q, Vector3(j.translate), Vector3(j.scale));
		inverseBaseFrame[i] = baseFrame[i]; inverseBaseFrame[i].Invert();

		if (j.parent >= 0)
		{
			baseFrame[i] = baseFrame[j.parent] * baseFrame[i];
			inverseBaseFrame[i] *= inverseBaseFrame[j.parent];
		}
	}
	//load materials (and textures)
	/*for (unsigned i = 0; i < Temp.header.numMeshes; i++)
	{
		auto& m = Temp.meshes[i];
		printf("%s: loaded mesh: %s\n", Temp.file, &str[m.name]);
		textures[i] = loadtexture(&str[m.material], 0);
		if (textures[i]) printf("%s: loaded material: %s\n", filename, &str[m.material]);
	}*/

	return true;
}
bool LoadAnimations(IqmTemp& Temp) { return true; }
//
//bool LoadAnimations(IqmTemp& Temp)
//{
//	if (Temp.header.numJoints > 0)
//	{
//		if (skel->numbones <= 0)
//		{
//			skel->numbones = Temp.header.numjoints;
//			skel->bones = new boneinfo[skel->numbones];
//			for (int i = 0; i < Temp.header.numJoints; i++)
//			{
//				joint &j = joints[i];
//				boneinfo &b = skel->bones[i];
//				if (!b.name) b.name = newstring(&str[j.name]);
//				b.parent = j.parent;
//				if (skel->shared <= 1)
//				{
//					j.pos.y = -j.pos.y;
//					j.orient.x = -j.orient.x;
//					j.orient.z = -j.orient.z;
//					j.orient.normalize();
//					b.base = dualquat(j.orient, j.pos);
//					if (b.parent >= 0) b.base.mul(skel->bones[b.parent].base, dualquat(b.base));
//					(b.invbase = b.base).invert();
//				}
//			}
//		}
//
//		if (skel->shared <= 1)
//			skel->linkchildren();
//	}
//
//	for (int i = 0; i < Temp.header.numMeshes; i++)
//	{
//		mesh &im = imeshes[i];
//		skelmesh* m = new skelmesh;
//		m->group = this;
//		meshes.add(m);
//		m->name = newstring(&str[im.name]);
//		m->numverts = im.numvertexes;
//		int noblend = -1;
//		if (m->numverts)
//		{
//			m->verts = new vert[m->numverts];
//			if (vtan) m->bumpverts = new bumpvert[m->numverts];
//			if (!vindex || !vweight)
//			{
//				blendcombo c;
//				c.finalize(0);
//				noblend = m->addblendcombo(c);
//			}
//		}
//		int fv = im.firstIQM_vertex;
//		float* mpos = vpos + 3* fv,
//			* mnorm = vnorm ? vnorm + 3* fv : NULL,
//			* mtan = vtan ? vtan + 4* fv : NULL,
//			* mtc = vtc ? vtc + 2* fv : NULL;
//		uchar* mindex = vindex ? vindex + 4* fv : NULL,* mweight = vweight ? vweight + 4* fv : NULL;
//
//		for (int j = 0; j < im.numVertexes; j++)
//		{
//			vert &v = m->verts[j];
//			v.pos = vec(mpos[0], -mpos[1], mpos[2]);
//			mpos += 3;
//			if (mtc)
//			{
//				v.u = mtc[0];
//				v.v = mtc[1];
//				mtc += 2;
//			}
//			else v.u = v.v = 0;
//			if (mnorm)
//			{
//				v.norm = vec(mnorm[0], -mnorm[1], mnorm[2]);
//				mnorm += 3;
//				if (mtan)
//				{
//					bumpvert &bv = m->bumpverts[j];
//					bv.tangent = vec(mtan[0], -mtan[1], mtan[2]);
//					bv.bitangent = mtan[3];
//					mtan += 4;
//				}
//			}
//			else v.norm = vec(0, 0, 0);
//			if (noblend < 0)
//			{
//				blendcombo c;
//				int sorted = 0;
//				loopk(4) sorted = c.addweight(sorted, mweight[k], mindex[k]);
//				mweight += 4;
//				mindex += 4;
//				c.finalize(sorted);
//				v.blend = m->addblendcombo(c);
//			}
//			else v.blend = noblend;
//		}
//		m->numtris = im.numtriangles;
//		if (m->numtris) m->tris = new tri[m->numtris];
//		triangle* mtris = tris + im.firstIQM_triangle;
//		loopj(im.numtriangles)
//		{
//			tri &t = m->tris[j];
//			t.vert[0] = mtris->vertex[0] - fv;
//			t.vert[1] = mtris->vertex[1] - fv;
//			t.vert[2] = mtris->vertex[2] - fv;
//			++mtris;
//		}
//		if (!m->numtris || !m->numverts)
//		{
//			conoutf("empty mesh in %s", filename);
//			meshes.removeobj(m);
//			delete m;
//		}
//	}
//
//	sortblendcombos();
//	
//	return true;
//}