#include "Pch.hpp"
#include "PositionColorShader.hpp"
#include "App/SystemSettings.hpp"

using namespace Shaders;

const D3D11_INPUT_ELEMENT_DESC PositionColorShader::Vertex::ElementDesc[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};
const unsigned PositionColorShader::Vertex::ElementCount = ARRAYSIZE(PositionColorShader::Vertex::ElementDesc);

PositionColorShader::PositionColorShader(const DeviceResourcesPtr& DeviceResources)
	: wvp(DeviceResources), vshader(), pshader()
{
	auto file = Sys::ReadFileAsync(StringOps::ToWString(SystemSettings::GetShaderFile("PositionColor.vs.cso"))).then([this, &DeviceResources](const Sys::FileData& Data)
	{
		vshader = VertexShader(DeviceResources, Data);
		DeviceResources->GetD3DDevice()->CreateInputLayout(Vertex::ElementDesc, Vertex::ElementCount, Data.data(), Data.size(), inputLayout.GetAddressOf());
	});
	file = Sys::ReadFileAsync(StringOps::ToWString(SystemSettings::GetShaderFile("PositionColor.ps.cso"))).then([this, &DeviceResources](const Sys::FileData& Data)
	{
		pshader = PixelShader(DeviceResources, Data);
	});
}