#include "Pch.hpp"
#include "Texture2D.hpp"
#include "Formats/DDSTextureLoader.hpp"
//#include "Formats/WICTextureLoader.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "Formats/StbImage.hpp"
#include "Common/PlatformHelpers.hpp"

using namespace DirectX;

bool Texture2D::IsTextureFile(const std::wstring& Extension)
{
	return (
		   Extension == L"png"
		|| Extension == L"tga"
		|| Extension == L"dds"
		|| Extension == L"jpg"
		|| Extension == L"bmp"
		|| Extension == L"psd"
		|| Extension == L"gif"
		|| Extension == L"hdr"
		|| Extension == L"pic"
		|| Extension == L"pnm"
	);
}

Texture2D::Texture2D(const DeviceResourcesPtr& DeviceResources, const std::wstring& File, bool AllowWrites)
	: Texture(DeviceResources, nullptr)
{
	if (StringEndsWith(File, L".dds")) //load with DDS loader
	{
		ID3D11Resource* tex;
		Sys::ThrowIfFailed
		(
			CreateDDSTextureFromFileEx
			(
				DeviceResources->GetD3DDevice(),
				File.data(),
				0,
				(AllowWrites ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT),
				D3D11_BIND_SHADER_RESOURCE,
				(AllowWrites ? D3D11_CPU_ACCESS_WRITE : 0),
				0,
				false,
				&tex,
				&srv
			)
		);
		static_cast<ID3D11Texture2D*>(tex)->GetDesc(&desc);
		tex->Release();
	}

	//else //use WIC (Does not, by default, support TGA)
	//	Sys::ThrowIfFailed(CreateWICTextureFromFile(DeviceResources->GetD3DDevice(), File.data(), &tex, &srv));

	else //use STB_Image
	{
		int x, y, n;
		auto fs = std::string(File.begin(), File.end());
		const char* file = fs.data();
		uint8_t* data = stbi_load(file, &x, &y, &n, 4);
		if (data == nullptr)
			throw L"Error loading texture: " + File;

		LoadFromData(data, x, y, AllowWrites);
	}

}

Texture2D::Texture2D(const DeviceResourcesPtr& DeviceResources, const ComPtr<ID3D11ShaderResourceView>& ShaderResourceView)
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

Texture2D::Texture2D(const DeviceResourcesPtr& DeviceResources, unsigned Width, unsigned Height, DXGI_FORMAT Format, unsigned MipLevels, bool AllowWrites)
	: Texture(DeviceResources, nullptr)
{
	ID3D11Texture2D* tex = nullptr;
	ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));

	desc.Format = Format;
	desc.Width = Width;
	desc.Height = Height;
	desc.MipLevels = desc.ArraySize = MipLevels;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.Usage = (AllowWrites ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT);
	desc.CPUAccessFlags = (AllowWrites ? D3D11_CPU_ACCESS_WRITE : 0);
	desc.MiscFlags = 0;

	Sys::ThrowIfFailed(DeviceResources->GetD3DDevice()->CreateTexture2D(&desc, nullptr, &tex));
	Sys::ThrowIfFailed(DeviceResources->GetD3DDevice()->CreateShaderResourceView(tex, nullptr, &srv));
	tex->Release();
}

void Texture2D::LoadFromData(uint8_t* Data, unsigned Width, unsigned Height, bool AllowWrites)
{
	ID3D11Texture2D* tex = nullptr;

	ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
	desc.Format = DXGI_FORMAT_R8G8B8A8_UINT;
	desc.Width = Width;
	desc.Height = Height;
	desc.MipLevels = desc.ArraySize = 1;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.Usage = (AllowWrites ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT);
	desc.CPUAccessFlags = (AllowWrites ? D3D11_CPU_ACCESS_WRITE : 0);
	desc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA sub;
	sub.pSysMem = Data;
	sub.SysMemPitch = Width * sizeof(uint32_t);
	sub.SysMemSlicePitch = 0;

	Sys::ThrowIfFailed(deviceResources->GetD3DDevice()->CreateTexture2D(&desc, &sub, &tex));
	Sys::ThrowIfFailed(deviceResources->GetD3DDevice()->CreateShaderResourceView(tex, nullptr, &srv));

	tex->GetDesc(&desc);
	tex->Release();
}