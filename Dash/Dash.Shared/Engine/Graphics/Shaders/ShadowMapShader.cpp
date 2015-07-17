#include "Pch.hpp"
#include "ShadowMapShader.hpp"
#include "App/AppData.hpp"

using namespace Shaders;

const D3D11_INPUT_ELEMENT_DESC ShadowMapShader::Vertex::ElementDesc[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};
const unsigned ShadowMapShader::Vertex::ElementCount = ARRAYSIZE(ShadowMapShader::Vertex::ElementDesc);

ShadowMapShader::ShadowMapShader(const DeviceResourcesPtr& DeviceResources)
	: world(DeviceResources), light(DeviceResources), vshader(), pshader()
{
	auto fp = AppData::GetShaderFile(L"ShadowMap.vs.cso");
	auto file = Sys::ReadFileAsync(fp).then([this, &DeviceResources](const Sys::FileData& Data)
	{
		vshader = VertexShader(DeviceResources, Data);
		DeviceResources->GetD3DDevice()->CreateInputLayout(Vertex::ElementDesc, Vertex::ElementCount, Data.data(), Data.size(), inputLayout.GetAddressOf());
	});
	fp = AppData::GetShaderFile(L"ShadowMap.ps.cso");
	file = Sys::ReadFileAsync(fp).then([this, &DeviceResources](const Sys::FileData& Data)
	{
		pshader = PixelShader(DeviceResources, Data);
	});
}