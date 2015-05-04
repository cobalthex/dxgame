#include "Pch.hpp"
#include "ObjLoader.hpp"
#include "Common/StreamOps.hpp"
#include "Graphics/Models/MeshOps.hpp"
#include "App/SystemSettings.hpp"

typedef std::vector<StaticModel::VertexType> VList;
typedef std::vector<StaticModel::IndexType> IList;

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

bool LoadMaterials(const std::string& LibFile, TextureCache& TexCache, std::map<std::string, Materials::LitMaterial>& Materials, const std::shared_ptr<Shaders::LitShader>& Shader); //load a list of materials from a file into the materials list

bool Obj::Load(const DeviceResourcesPtr& DeviceResources, const std::string& Filename, TextureCache& TexCache, const std::shared_ptr<Shaders::LitShader>& Shader, __out StaticModel& Model)
{
	std::vector<Vector3> positions;
	std::vector<Vector3> normals;
	std::vector<Vector2> texcoords;

	std::map<VertexIndex, unsigned> uVerts;

	VList vertices;
	IList indices;

	std::map<std::string, Materials::LitMaterial> materials;

	std::string curMaterial; //currently selected material
	std::string name; //current object name

	std::map<std::string, StaticModel::MeshType> meshes;
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
				Materials::LitMaterial mtl;
				if (materials.find(curMaterial) != materials.end())
					mtl = materials[curMaterial];
				else
					mtl.shader = Shader;

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
			ty = SystemSettings::GetMaterialFile(ty);
			LoadMaterials(ty, TexCache, materials, Shader);
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

				StreamOps::SkipWhitespace(fin);

				fin >> vi.v;
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

				//vertex already exists
				auto uvi = uVerts.find(vi);
				if (uvi != uVerts.end())
					indices.push_back(uvi->second);
				else
				{
					auto idx = (unsigned)vertices.size();
					indices.push_back(idx);
					uVerts[vi] = idx;

					//obj is 1 based
					vi.v--;
					vi.t--;
					vi.n--;

					vi.v = vi.v < 0 ? (int)positions.size() - vi.v : vi.v;
					vi.n = vi.n < 0 ? (int)normals.size() - vi.n : vi.n;
					vi.t = vi.t < 0 ? (int)texcoords.size() - vi.t : vi.t;

					StaticModel::VertexType vtx;
					if (positions.size() > vi.v)
						vtx.position = positions[vi.v];
					if (isN && normals.size() > vi.n)
						vtx.normal = normals[vi.n];
					if (isT && texcoords.size() > vi.t)
						vtx.texCoord = texcoords[vi.t];

					vertices.push_back(vtx);
				}
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
		Materials::LitMaterial mtl;
		if (materials.find(curMaterial) != materials.end())
			mtl = materials[curMaterial];
		else
			mtl.shader = Shader;

		meshes[name] = { name, nextVertex, (unsigned)vertices.size() - nextVertex, nextIndex, (unsigned)indices.size() - nextIndex, mtl, Bounds() };
	}

	Model = StaticModel(DeviceResources, vertices, indices, PrimitiveTopology::TriangleList, meshes);

	return true;
}

bool LoadMaterials(const std::string& LibFile, TextureCache& TexCache, std::map<std::string, Materials::LitMaterial>& Materials, const std::shared_ptr<Shaders::LitShader>& Shader)
{
	std::ifstream fin(LibFile);
	if (!fin.is_open())
		return false;

	Color ambient;
	Color diffuse;
	Color emissive;
	Color specular;
	float specPower = 0;

	int illum = 2;
	float dissolved = 1;

	std::string name;

	std::shared_ptr<Texture2D> diffMap; //others unused currently

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

		if (ty == "newmtl") //no empty named objects
		{
			if (name.length() > 0)
			{
				Materials[name] = Materials::LitMaterial(Shader);
				auto& mtl = Materials[name];
				mtl.ambient = ambient;
				mtl.diffuse = diffuse;
				mtl.diffuseMap = diffMap;
				mtl.useTexture = (diffMap != nullptr && diffMap->IsValid());
				mtl.emissive = emissive;
				mtl.specular = specular;
				mtl.specularPower = specPower;
			}
			ambient = diffuse = emissive = specular = Color();
			specPower = 0;
			illum = 2;
			dissolved = 1;
			diffMap = nullptr;

			StreamOps::SkipWhitespace(fin);
			std::getline(fin, name);
		}
		else if (ty == "ka")
			fin >> ambient.x >> ambient.y >> ambient.z;
		else if (ty == "kd")
			fin >> diffuse.x >> diffuse.y >> diffuse.z;
		else if (ty == "ke")
			fin >> emissive.x >> emissive.y >> emissive.z;
		else if (ty == "ks")
			fin >> specular.x >> specular.y >> specular.z;
		else if (ty == "ns")
			fin >> specPower;
		else if (ty == "illum")
			fin >> illum;
		else if (ty == "d" || ty == "tr")
			fin >> dissolved;
		else if (ty == "map_kd") //options not supported
		{
			StreamOps::SkipWhitespace(fin);
			std::getline(fin, ty);

			diffMap = TexCache.Load(ty);
		}
		else //all others unused
			std::getline(fin, ty);
	}
	fin.close();

	//last material
	if (name.length() > 0)
	{
		Materials[name] = Materials::LitMaterial(Shader);
		auto& mtl = Materials[name];
		mtl.ambient = ambient;
		mtl.diffuse = diffuse;
		mtl.diffuseMap = diffMap;
		mtl.useTexture = (diffMap != nullptr && diffMap->IsValid());
		mtl.emissive = emissive;
		mtl.specular = specular;
		mtl.specularPower = specPower;
	}

	return true;
}