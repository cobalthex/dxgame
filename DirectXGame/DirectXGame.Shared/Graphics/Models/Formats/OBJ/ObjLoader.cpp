#include "Pch.hpp"
#include "ObjLoader.hpp"
#include "Common/StreamOps.hpp"

bool LoadMaterials(const std::string& LibFile, TextureCache& TexCache, std::map<std::string, Materials::LitMaterial>& Materials, const std::shared_ptr<Shaders::LitShader>& Shader); //load a list of materials from a file into the materials list

bool Obj::Load(const DeviceResourcesPtr& DeviceResources, const std::string& Filename, TextureCache& TexCache, const std::shared_ptr<Shaders::LitShader>& Shader, __out StaticModel& Model)
{
	std::vector<Vector3> positions;
	std::vector<Vector3> normals;
	std::vector<Vector2> texcoords;

	std::vector<StaticModel::VertexType> vertices;
	std::vector<StaticModel::IndexType> indices;

	std::map<std::string, Materials::LitMaterial> materials;

	std::string curMaterial; //currently selected material
	std::string name; //current object name

	std::vector<StaticModel::MeshType> meshes;
	unsigned nextVertex = 0, nextIndex = 0;

	std::ifstream fin(Filename);
	if (!fin.is_open())
		return false;

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

				StaticModel::MeshType m(name, nextVertex, (unsigned)vertices.size() - nextVertex, nextIndex, (unsigned)indices.size() - nextIndex, mtl, Bounds());

				meshes.push_back(m);
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
			getline(fin, ty);
			int i;
			for (i = 0; i < ty.length() && ty[i] < 33; i++); //remove leading whitespace

			LoadMaterials(ty.substr(i - 1), TexCache, materials, Shader);
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
			int v = 0, t = 0, n = 0;
			bool isT = false, isN = false;

			while (!fin.eof() && fin.peek() != '\n')
			{
				StreamOps::SkipWhitespace(fin);

				v = StreamOps::ReadInt(fin);
				if (fin.peek() == '/')
				{
					fin.get();
					t = StreamOps::ReadInt(fin);
					isT = true;
				}
				if (fin.peek() == '/')
				{
					fin.get();
					n = StreamOps::ReadInt(fin);
					isN = true;
				}

				v--;
				t--;
				n--;

				StaticModel::VertexType vtx;

				if (positions.size() > v)
					vtx.position = positions[v < 0 ? positions.size() - v : v];
				if (isN && normals.size() > n)
					vtx.normal = normals[n < 0 ? normals.size() - n : n];
				if (isT && texcoords.size() > t)
					vtx.texCoord = texcoords[t < 0 ? texcoords.size() - t : t];

				vertices.push_back(vtx);
				indices.push_back((unsigned)vertices.size() - 1);
			}
		}
		else if (ty == "usemtl")
		{
			getline(fin, ty);
			int i;
			for (i = 0; i < ty.length() && ty[i] < 33; i++); //remove leading whitespace
			curMaterial = ty.substr(i - 1);
		}
		else //unused types
			std::getline(fin, ty);
	}

	fin.close();

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
	return true;
}