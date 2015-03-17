#include "Pch.hpp"
#include "PositionColorShader.hpp"

using namespace Shaders;

PositionColorShader::PositionColorShader(const DX::DeviceResourcesPtr& DeviceResources)
	: wvp(), vshader(), pshader()
{
	auto file = Sys::ReadFileAsync(L"PositionColor.vs.cso").then([this, &DeviceResources](const Sys::FileData& Data)
	{
		vshader = VertexShader(DeviceResources, Data);
		DeviceResources->GetD3DDevice()->CreateInputLayout(Vertex::ElementDesc, Vertex::ElementCount, Data.data(), Data.size(), inputLayout.GetAddressOf());
	});
	file = Sys::ReadFileAsync(L"PositionColor.ps.cso").then([this, &DeviceResources](const Sys::FileData& Data)
	{
		pshader = PixelShader(DeviceResources, Data);
	});
}