#include "Pch.hpp"
#include "StaticUnlitShader.hpp"
#include "Engine/Graphics/Models/VertexFormats.hpp"
#include "App/AppData.hpp"

using namespace Shaders;

StaticUnlitShader::StaticUnlitShader(const DeviceResourcesPtr& DeviceResources)
	: object(DeviceResources), vshader(), pshader()
{
	auto fp = StringOps::ToWString(AppData::GetShaderFile("StaticLit.vs.cso"));
	auto file = Sys::ReadFileAsync(fp).then([this, &DeviceResources](const Sys::FileData& Data)
	{
		vshader = VertexShader(DeviceResources, Data);
		DeviceResources->GetD3DDevice()->CreateInputLayout(Vertexes::StaticVertex::ElementDesc, Vertexes::StaticVertex::ElementCount, Data.data(), Data.size(), inputLayout.GetAddressOf());
	});
	fp = StringOps::ToWString(AppData::GetShaderFile("StaticUnit.ps.cso"));
	file = Sys::ReadFileAsync(fp).then([this, &DeviceResources](const Sys::FileData& Data)
	{
		pshader = PixelShader(DeviceResources, Data);
	});
}