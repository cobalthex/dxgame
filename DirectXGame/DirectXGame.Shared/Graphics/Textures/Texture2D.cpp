#include "Pch.hpp"
#include "Texture2D.hpp"
#include "Formats/DDSTextureLoader.hpp"
#include "Formats/WICTextureLoader.hpp"
#include "Common/PlatformHelpers.hpp"

using namespace DirectX;

inline bool EndsWith(const std::wstring& String, const std::wstring& Suffix)
{
	if (Suffix.size() > String.size())
		return false;
	return std::equal(Suffix.rbegin(), Suffix.rend(), String.rbegin());
}

Texture2D::Texture2D(const DX::DeviceResourcesPtr& DeviceResources, const std::wstring& File)
	: Texture(DeviceResources, nullptr)
{
	//texture info
	ComPtr<ID3D11Resource> tex = nullptr;

	//tga not supported

	if (EndsWith(File, L".dds")) //load with DDS loader
		DX::ThrowIfFailed(CreateDDSTextureFromFile(DeviceResources->GetD3DDevice(), File.data(), &tex, &srv));

	else //use WIC
		DX::ThrowIfFailed(CreateWICTextureFromFile(DeviceResources->GetD3DDevice(), File.data(), &tex, &srv));

	static_cast<ID3D11Texture2D*>(tex.Get())->GetDesc(&desc);
}

Texture2D::Texture2D(const DX::DeviceResourcesPtr& DeviceResources, const ComPtr<ID3D11ShaderResourceView>& ShaderResourceView)
	: Texture(DeviceResources, ShaderResourceView)
{
	ComPtr<ID3D11Resource> tex = nullptr;
	srv->GetResource(&tex);
	static_cast<ID3D11Texture2D*>(tex.Get())->GetDesc(&desc);
}

Texture2D& Texture2D::operator=(const Texture2D& Copy)
{
	if (this == &Copy)
		return *this;

	srv = Copy.srv;
	deviceResources = Copy.deviceResources;
	desc = Copy.desc;

	return *this;
}

Texture2D::Texture2D(const DX::DeviceResourcesPtr& DeviceResources, unsigned Width, unsigned Height, DXGI_FORMAT Format, unsigned MipLevel)
	: Texture(DeviceResources, nullptr)
{
	ID3D11Texture2D* tex = nullptr;
	ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));

	desc.Format = Format;
	desc.Width = Width;
	desc.Height = Height;
	desc.MipLevels = desc.ArraySize = MipLevel;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	if (SUCCEEDED(DeviceResources->GetD3DDevice()->CreateTexture2D(&desc, nullptr, &tex)))
	{
		if (FAILED(DeviceResources->GetD3DDevice()->CreateShaderResourceView(tex, nullptr, &srv)))
			srv = nullptr;
		tex->Release();
	}
}