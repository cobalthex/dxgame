#include "Pch.hpp"
#include "ObjLoader.hpp"
#include "Engine/Common/StreamOps.hpp"
#include "Engine/Graphics/Models/MeshOps.hpp"
#include "App/AppData.hpp"
#include "Engine/Graphics/Material.hpp"

//triangle for obj vertex indices
struct VertexIndex
{
	int v, t, n;
	inline bool operator < (const VertexIndex& Other) const
	{
		if (v != Other.v)
			return (v < Other.v);
		if (t != Other.t)
			return (t < Other.t);
		if (n != Other.n)
			return (n < Other.n);
		return false;
	}
};

bool Obj::Load(const DeviceResourcesPtr& DeviceResources, const std::string& Filename, TextureCache& TexCache, __out Model& Model)
{
	std::vector<Vector3> positions;
	std::vector<Vector3> normals;
	std::vector<Vector2> texcoords;

	std::map<VertexIndex, unsigned> uVerts;

	std::vector<Model::VertexType> vertices;
	std::vector<Model::IndexType> indices;

	std::map<std::string, Material> materials;

	std::string curMaterial; //currently selected material
	std::string name; //current object name

	std::map<std::string, ModelMesh> meshes;
	unsigned nextVertex = 0, nextIndex = 0;

	std::ifstream fin(Filename);
	if (!fin.is_open())
		return false;
	unsigned t = 0;
	std::string ty;
	while (!fin.eof())
	{
		StreamOps::SkipWhitespace(fin);
		if (fin.peek() == '#')
		{
			std::getline(fin, ty);
			continue;
		}
		fin >> ty; //type
		ty = StringOps::ToLower(ty);

		if (ty == "g" || ty == "o") //objects and groups treated equally
		{
			//first object or empty
			if (vertices.size() > 0)
			{
				Material mtl;
				if (materials.find(curMaterial) != materials.end())
					mtl = materials[curMaterial];

				meshes[name] = { name, nextVertex, (unsigned)vertices.size() - nextVertex, nextIndex, (unsigned)indices.size() - nextIndex, mtl, Bounds() };
				
				nextVertex = (unsigned)vertices.size();
				nextIndex = (unsigned)indices.size();
			}

			curMaterial.clear();

			StreamOps::SkipWhitespace(fin);
			getline(fin, name);
			continue;
		}
		else if (ty == "mtllib")
		{
			StreamOps::SkipWhitespace(fin);
			getline(fin, ty);
			ty = AppData::GetMaterialFile(ty);
			materials = Material::LoadAllFromFile(ty, TexCache);
		}
		else if (ty == "v")
		{
			Vector3 v;
			fin >> v.x >> v.y >> v.z;
			positions.push_back(v);
		}
		else if (ty == "vn")
		{
			Vector3 n;
			fin >> n.x >> n.y >> n.z;
			normals.push_back(n);
		}
		else if (ty == "vt")
		{
			Vector2 t;
			fin >> t.x >> t.y;
			texcoords.push_back(t);
		}
		else if (ty == "f")
		{
			VertexIndex vi;
			bool isT = false, isN = false;

			size_t fc = 0;
			while (!fin.eof() && fin.peek() != '\n')
			{
				fc++;
				if (fc > 3)
				{
					std::getline(fin, ty);
					break; //todo
				}

				StreamOps::SkipWhitespace(fin);

				fin >> vi.v;
				isT = false;
				isN = false;
				if (fin.peek() == '/')
				{
					fin.ignore();
					fin >> vi.t;
					isT = true;
				}
				if (fin.peek() == '/')
				{
					fin.ignore();
					fin >> vi.n;
					isN = true;
				}
				
				//invalid vertices, skip this face
				if (vi.v > (int)positions.size() || (isN && vi.n > (int)normals.size()) || (isT && vi.t > (int)texcoords.size()))
				{
					std::getline(fin, ty);
					break;
				}

				auto uvi = uVerts.find(vi);
				if (uvi == uVerts.end()) //not found
				{
					Model::VertexType vtx;
					vtx.position = positions[vi.v < 0 ? positions.size() + vi.v : vi.v - 1];
					
					if (isN)
						vtx.normal = normals[vi.n < 0 ? normals.size() + vi.n : vi.n - 1];
					else
						vtx.normal = Vector3::Zero;

					if (isT)
						vtx.texCoord = texcoords[vi.t < 0 ? texcoords.size() + vi.t : vi.t - 1];
					else
						vtx.texCoord = Vector2::Zero;

					uVerts[vi] = (unsigned)vertices.size();
					vertices.push_back(vtx);
					//indices.push_back(uVerts[vi]);
				}
				else
					indices.push_back(uvi->second);
			}
		}
		else if (ty == "usemtl")
		{
			StreamOps::SkipWhitespace(fin);
			getline(fin, ty);
			curMaterial = ty;
		}
		else //unused types
			std::getline(fin, ty);
	}

	fin.close();

	//last mesh
	if (vertices.size() > 0)
	{
		Material mtl;
		if (materials.find(curMaterial) != materials.end())
			mtl = materials[curMaterial];

		meshes[name] = { name, nextVertex, (unsigned)vertices.size() - nextVertex, nextIndex, (unsigned)indices.size() - nextIndex, mtl, Bounds() };
	}

	Model = ::Model(DeviceResources, vertices, indices, PrimitiveTopology::TriangleList, meshes);

	return true;
}