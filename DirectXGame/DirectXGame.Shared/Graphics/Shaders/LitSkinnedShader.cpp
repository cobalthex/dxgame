#include "Pch.hpp"
#include "LitSkinnedShader.hpp"

using namespace Shaders;

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
	auto file = Sys::ReadFileAsync(L"LitSkinned.vs.cso").then([this, &DeviceResources](const Sys::FileData& Data)
	{
		vshader = VertexShader(DeviceResources, Data);
		DeviceResources->GetD3DDevice()->CreateInputLayout(Vertex::ElementDesc, Vertex::ElementCount, Data.data(), Data.size(), inputLayout.GetAddressOf());
	});
	file = Sys::ReadFileAsync(L"LitSkinned.ps.cso").then([this, &DeviceResources](const Sys::FileData& Data)
	{
		pshader = PixelShader(DeviceResources, Data);
	});
}