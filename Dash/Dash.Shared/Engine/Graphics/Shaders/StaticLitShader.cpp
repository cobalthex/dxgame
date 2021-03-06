#include "Pch.hpp"
#include "StaticLitShader.hpp"
#include "Engine/Graphics/Models/VertexFormats.hpp"
#include "App/AppData.hpp"

using namespace Shaders;

StaticLitShader::StaticLitShader(const DeviceResourcesPtr& DeviceResources)
	: object(DeviceResources), material(DeviceResources), lighting(DeviceResources), vshader(), pshader()
{
	auto fp = StringOps::ToWString(AppData::GetShaderFile("StaticLit.vs.cso"));
	auto file = Sys::ReadFileAsync(fp).then([this, &DeviceResources](const Sys::FileData& Data)
	{
		vshader = VertexShader(DeviceResources, Data);
		DeviceResources->GetD3DDevice()->CreateInputLayout(Vertexes::StaticVertex::ElementDesc, Vertexes::StaticVertex::ElementCount, Data.data(), Data.size(), inputLayout.GetAddressOf());
	});
	fp = StringOps::ToWString(AppData::GetShaderFile("StaticLit.ps.cso"));
	file = Sys::ReadFileAsync(fp).then([this, &DeviceResources](const Sys::FileData& Data)
	{
		pshader = PixelShader(DeviceResources, Data);
	});
}

void StaticLitShader::ApplyMaterial(const Material& Material)
{
	material.data.ambient = Material.ambient;
	material.data.diffuse = Material.diffuse;
	material.data.emissive = Material.emissive;
	material.data.specular = Material.specular;
	material.data.specularPower = Material.specularPower;
	material.data.useTexture = Material.useTexture;

	material.BindPixel(0);
}