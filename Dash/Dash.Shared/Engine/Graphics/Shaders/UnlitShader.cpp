#include "Pch.hpp"
#include "UnlitShader.hpp"
#include "Engine/Data/Formats/Osl/Osl.hpp"
#include "App/AppData.hpp"
#include "Engine/Graphics/Shaders/LitShader.hpp"

using namespace Shaders;
using namespace Materials;

UnlitShader::UnlitShader(const DeviceResourcesPtr& DeviceResources)
	: object(DeviceResources), vshader(), pshader()
{
	auto fp = StringOps::ToWString(AppData::GetShaderFile("Lit.vs.cso"));
	auto file = Sys::ReadFileAsync(fp).then([this, &DeviceResources](const Sys::FileData& Data)
	{
		vshader = VertexShader(DeviceResources, Data);
		DeviceResources->GetD3DDevice()->CreateInputLayout(Vertex::ElementDesc, Vertex::ElementCount, Data.data(), Data.size(), inputLayout.GetAddressOf());
	});
	fp = StringOps::ToWString(AppData::GetShaderFile("Unlit.ps.cso"));
	file = Sys::ReadFileAsync(fp).then([this, &DeviceResources](const Sys::FileData& Data)
	{
		pshader = PixelShader(DeviceResources, Data);
	});
}