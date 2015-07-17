#include "Pch.hpp"
#include "Material.hpp"

Material Material::FromFile(const std::string& MaterialFile, TextureCache& TexCache)
{
	float r, g, b, a;

	Osl::ValueList col;

	if (MaterialObject.Contains("diffuse"))
	{
		col = MaterialObject["ambient"];
		r = (float)(col.size() > 0 ? col[0].ToFloat() : 0);
		g = (float)(col.size() > 1 ? col[1].ToFloat() : 0);
		b = (float)(col.size() > 2 ? col[2].ToFloat() : 0);
		a = (float)(col.size() > 3 ? col[3].ToFloat() : 1);
		ambient = Color(r, g, b, a);
	}
	if (MaterialObject.Contains("diffuse"))
	{
		col = MaterialObject["diffuse"];
		r = (float)(col.size() > 0 ? col[0].ToFloat() : 0);
		g = (float)(col.size() > 1 ? col[1].ToFloat() : 0);
		b = (float)(col.size() > 2 ? col[2].ToFloat() : 0);
		a = (float)(col.size() > 3 ? col[3].ToFloat() : 1);
		diffuse = Color(r, g, b, a);
	}
	if (MaterialObject.Contains("diffuse"))
	{
		col = MaterialObject["specular"];
		r = (float)(col.size() > 0 ? col[0].ToFloat() : 0);
		g = (float)(col.size() > 1 ? col[1].ToFloat() : 0);
		b = (float)(col.size() > 2 ? col[2].ToFloat() : 0);
		a = (float)(col.size() > 3 ? col[3].ToFloat() : 1);
		specular = Color(r, g, b, a);
		specularPower = (float)(col.size() > 4 ? col[4].ToFloat() : 0);
	}
	if (MaterialObject.Contains("diffuse"))
	{
		col = MaterialObject["emissive"];
		r = (float)(col.size() > 0 ? col[0].ToFloat() : 0);
		g = (float)(col.size() > 1 ? col[1].ToFloat() : 0);
		b = (float)(col.size() > 2 ? col[2].ToFloat() : 0);
		a = (float)(col.size() > 3 ? col[3].ToFloat() : 1);
		emissive = Color(r, g, b, a);
	}
	if (MaterialObject.Contains("diffmap") && MaterialObject["diffmap"].size() > 0)
		diffuseMap = TexCache.Load(MaterialObject["diffmap"][0]);
}