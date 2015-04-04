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
	: LitShader(DeviceResources, SystemSettings::GetShaderFile("LitSkinned.vs.cso"), SystemSettings::GetShaderFile("Lit.ps.cso"), Vertex::ElementDesc, Vertex::ElementCount) { }