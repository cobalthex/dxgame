#include "Pch.hpp"
#include "PositionTextureShader.hpp"
#include "App/AppData.hpp"

using namespace Shaders;

const D3D11_INPUT_ELEMENT_DESC PositionTextureShader::Vertex::ElementDesc[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};
const unsigned PositionTextureShader::Vertex::ElementCount = ARRAYSIZE(PositionTextureShader::Vertex::ElementDesc);

PositionTextureShader::PositionTextureShader(const DeviceResourcesPtr& DeviceResources)
	: wvp(DeviceResources), vshader(), pshader()
{
	auto file = Sys::ReadFileAsync(StringOps::ToWString(AppData::GetShaderFile("PositionTexture.vs.cso"))).then([this, &DeviceResources](const Sys::FileData& Data)
	{
		vshader = VertexShader(DeviceResources, Data);
		DeviceResources->GetD3DDevice()->CreateInputLayout(Vertex::ElementDesc, Vertex::ElementCount, Data.data(), Data.size(), inputLayout.GetAddressOf());
	});
	file = Sys::ReadFileAsync(StringOps::ToWString(AppData::GetShaderFile("PositionTexture.ps.cso"))).then([this, &DeviceResources](const Sys::FileData& Data)
	{
		pshader = PixelShader(DeviceResources, Data);
	});
}