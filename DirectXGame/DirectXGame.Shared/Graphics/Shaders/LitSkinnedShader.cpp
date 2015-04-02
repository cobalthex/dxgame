#include "Pch.hpp"
#include "LitSkinnedShader.hpp"
#include "Data/Formats/Osl/Osl.hpp"
#include "App/SystemSettings.hpp"

using namespace Shaders;
using namespace Materials;

const D3D11_INPUT_ELEMENT_DESC LitSkinnedShader::Vertex::ElementDesc[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "BLENDINDICES", 0, DXGI_FORMAT_R8G8B8A8_UINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "BLENDWEIGHT", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};
const unsigned LitSkinnedShader::Vertex::ElementCount = ARRAYSIZE(LitSkinnedShader::Vertex::ElementDesc);

LitSkinnedShader::LitSkinnedShader(const DeviceResourcesPtr& DeviceResources)
	: object(DeviceResources), material(DeviceResources), lighting(DeviceResources), vshader(), pshader()
{
	auto fp = ToWString(SystemSettings::GetShaderFile("LitSkinned.vs.cso"));
	auto file = Sys::ReadFileAsync(fp).then([this, &DeviceResources](const Sys::FileData& Data)
	{
		vshader = VertexShader(DeviceResources, Data);
		DeviceResources->GetD3DDevice()->CreateInputLayout(Vertex::ElementDesc, Vertex::ElementCount, Data.data(), Data.size(), inputLayout.GetAddressOf());
	});
	fp = ToWString(SystemSettings::GetShaderFile("LitSkinned.ps.cso"));
	file = Sys::ReadFileAsync(fp).then([this, &DeviceResources](const Sys::FileData& Data)
	{
		pshader = PixelShader(DeviceResources, Data);
	});
}

//Retrieve an OSL value as a float. Defaults to zero if invalid type
inline float OslValueToFloat(const Osl::Value& Value)
{
	switch (Value.Type())
	{
	case Osl::Types::Integer: return (float)((Osl::integer)Value);
	case Osl::Types::Decimal: return (float)((Osl::decimal)Value);
	default: return 0;
	}
}
LitSkinnedMaterial::LitSkinnedMaterial(TextureCache& TexCache, const Osl::Object& MaterialObject, const std::shared_ptr<Shaders::LitSkinnedShader>& Shader)
	: Material(Shader)
{
	float r, g, b, a;

	Osl::ValueList col;

	if (MaterialObject.Contains("diffuse"))
	{
		col = MaterialObject["ambient"];
		r = (float)(col.size() > 0 ? OslValueToFloat(col[0]) : 0);
		g = (float)(col.size() > 1 ? OslValueToFloat(col[1]) : 0);
		b = (float)(col.size() > 2 ? OslValueToFloat(col[2]) : 0);
		a = (float)(col.size() > 3 ? OslValueToFloat(col[3]) : 1);
		ambient = Color(r, g, b, a);
	}
	if (MaterialObject.Contains("diffuse"))
	{
		col = MaterialObject["diffuse"];
		r = (float)(col.size() > 0 ? OslValueToFloat(col[0]) : 0);
		g = (float)(col.size() > 1 ? OslValueToFloat(col[1]) : 0);
		b = (float)(col.size() > 2 ? OslValueToFloat(col[2]) : 0);
		a = (float)(col.size() > 3 ? OslValueToFloat(col[3]) : 1);
		diffuse = Color(r, g, b, a);
	}
	if (MaterialObject.Contains("diffuse"))
	{
		col = MaterialObject["specular"];
		r = (float)(col.size() > 0 ? OslValueToFloat(col[0]) : 0);
		g = (float)(col.size() > 1 ? OslValueToFloat(col[1]) : 0);
		b = (float)(col.size() > 2 ? OslValueToFloat(col[2]) : 0);
		a = (float)(col.size() > 3 ? OslValueToFloat(col[3]) : 1);
		specular = Color(r, g, b, a);
		specularPower = (float)(col.size() > 4 ? OslValueToFloat(col[4]) : 0);
	}
	if (MaterialObject.Contains("diffuse"))
	{
		col = MaterialObject["emissive"];
		r = (float)(col.size() > 0 ? OslValueToFloat(col[0]) : 0);
		g = (float)(col.size() > 1 ? OslValueToFloat(col[1]) : 0);
		b = (float)(col.size() > 2 ? OslValueToFloat(col[2]) : 0);
		a = (float)(col.size() > 3 ? OslValueToFloat(col[3]) : 1);
		emissive = Color(r, g, b, a);
	}
	if (MaterialObject.Contains("diffmap") && MaterialObject["diffmap"].size() > 0)
		diffuseMap = TexCache.Load(MaterialObject["diffmap"][0]);
}