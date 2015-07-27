#include "Pch.hpp"
#include "Material.hpp"
#include "Engine/Graphics/Textures/TextureCache.hpp"
#include "Engine/Common/PlatformHelpers.hpp"
#include "Engine/Common/StreamOps.hpp"
#include "Engine/Data/Formats/Osl/Osl.hpp"
#include "Engine/Graphics/Models/VertexFormats.hpp"
#include "Engine/Graphics/Shaders/Shader.hpp"

ShaderType Material::GetShaderType(VertexFormats VertexFormat) const
{
	if (!useLighting)
		return (VertexFormat == VertexFormats::Skinned ? ShaderType::SkinnedUnlit : ShaderType::StaticUnlit);
	else
		return (VertexFormat == VertexFormats::Skinned ? ShaderType::SkinnedLit : ShaderType::StaticLit);
}

std::map<std::string, Material> Material::LoadAllFromFile(const std::string& MaterialFile, TextureCache& TexCache)
{
	std::map<std::string, Material> matMap;

	float r, g, b, a;

	auto ext = Sys::GetFileExtension(MaterialFile);

	//OBJ mtl file
	if (ext == "mtl")
	{
		std::ifstream fin(MaterialFile);
		if (!fin.is_open())
			return matMap;

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
					Material mtl;
					mtl.ambient = ambient;
					mtl.diffuse = diffuse;
					mtl.texture = diffMap;
					mtl.useTexture = (diffMap != nullptr && diffMap->IsValid());
					mtl.emissive = emissive;
					mtl.specular = specular;
					mtl.specularPower = specPower;
					matMap[name] = mtl;
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
			Material mtl;
			mtl.ambient = ambient;
			mtl.diffuse = diffuse;
			mtl.texture = diffMap;
			mtl.useTexture = (diffMap != nullptr && diffMap->IsValid());
			mtl.emissive = emissive;
			mtl.specular = specular;
			mtl.specularPower = specPower;
			matMap[name] = mtl;
		}
	}
	//OSL based material file
	else if (ext == "matl")
	{
		auto doc = Osl::Document(MaterialFile);
		for (auto& obj : doc.objects)
		{
			auto& prop = obj.second;

			Material mtl;
			if (prop.Contains("diffuse"))
			{
				auto& col = prop["ambient"];
				r = (float)(col.size() > 0 ? col[0].ToFloat() : 0);
				g = (float)(col.size() > 1 ? col[1].ToFloat() : 0);
				b = (float)(col.size() > 2 ? col[2].ToFloat() : 0);
				a = (float)(col.size() > 3 ? col[3].ToFloat() : 1);
				mtl.ambient = Color(r, g, b, a);
			}
			if (prop.Contains("diffuse"))
			{
				auto& col = prop["diffuse"];
				r = (float)(col.size() > 0 ? col[0].ToFloat() : 0);
				g = (float)(col.size() > 1 ? col[1].ToFloat() : 0);
				b = (float)(col.size() > 2 ? col[2].ToFloat() : 0);
				a = (float)(col.size() > 3 ? col[3].ToFloat() : 1);
				mtl.diffuse = Color(r, g, b, a);
			}
			if (prop.Contains("diffuse"))
			{
				auto& col = prop["specular"];
				r = (float)(col.size() > 0 ? col[0].ToFloat() : 0);
				g = (float)(col.size() > 1 ? col[1].ToFloat() : 0);
				b = (float)(col.size() > 2 ? col[2].ToFloat() : 0);
				a = (float)(col.size() > 3 ? col[3].ToFloat() : 1);
				mtl.specular = Color(r, g, b, a);
				mtl.specularPower = (float)(col.size() > 4 ? col[4].ToFloat() : 0);
			}
			if (prop.Contains("diffuse"))
			{
				auto& col = prop["emissive"];
				r = (float)(col.size() > 0 ? col[0].ToFloat() : 0);
				g = (float)(col.size() > 1 ? col[1].ToFloat() : 0);
				b = (float)(col.size() > 2 ? col[2].ToFloat() : 0);
				a = (float)(col.size() > 3 ? col[3].ToFloat() : 1);
				mtl.emissive = Color(r, g, b, a);
			}
			if (prop.Contains("diffmap") && prop["diffmap"].size() > 0)
				mtl.texture = TexCache.Load(prop["diffmap"][0]);
			mtl.useTexture = (mtl.texture != nullptr && mtl.texture->IsValid());

			matMap[obj.first] = mtl;
		}
	}
	return matMap;
}