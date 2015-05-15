#include "Pch.hpp"
#include "PositionTextureShader.hpp"
#include "App/SystemSettings.hpp"
#include "TextShader.hpp"

using namespace Shaders;

TextShader::TextShader(const DeviceResourcesPtr& DeviceResources)
	: wvp(DeviceResources), color(DeviceResources), vshader(), pshader()
{
	auto file = Sys::ReadFileAsync(StringOps::ToWString(SystemSettings::GetShaderFile("PositionTexture.vs.cso"))).then([this, &DeviceResources](const Sys::FileData& Data)
	{
		vshader = VertexShader(DeviceResources, Data);
		DeviceResources->GetD3DDevice()->CreateInputLayout(Vertex::ElementDesc, Vertex::ElementCount, Data.data(), Data.size(), inputLayout.GetAddressOf());
	});
	file = Sys::ReadFileAsync(StringOps::ToWString(SystemSettings::GetShaderFile("Text.ps.cso"))).then([this, &DeviceResources](const Sys::FileData& Data)
	{
		pshader = PixelShader(DeviceResources, Data);
	});
};