#include "Pch.hpp"
#include "StaticLitShader.hpp"
#include "Engine/Data/Formats/Osl/Osl.hpp"
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