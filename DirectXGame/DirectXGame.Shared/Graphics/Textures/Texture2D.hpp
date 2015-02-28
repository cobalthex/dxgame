#pragma once

#include "Pch.hpp"
#include "Graphics/DeviceResources.hpp"
#include "Texture.hpp"

class Texture2D : public Texture
{
public:
	Texture2D(const DX::DeviceResourcesPtr& DeviceResources, unsigned Width, unsigned Height, DXGI_FORMAT Format = DXGI_FORMAT_R8G8B8A8_UINT, unsigned MipLevels = 1, bool AllowWrites = false); //Create an empty texture
	Texture2D(const DX::DeviceResourcesPtr& DeviceResources, const ComPtr<ID3D11ShaderResourceView>& ShaderResourceView);

	Texture2D::Texture2D(const DX::DeviceResourcesPtr& DeviceResources, const std::string& File, bool AllowWrites = false)
		: Texture2D(DeviceResources, std::wstring(File.begin(), File.end()), AllowWrites) { } //Load a texture from a file. DDS and formats in the WIC are supported
	Texture2D(const DX::DeviceResourcesPtr& DeviceResources, const std::wstring& File, bool AllowWrites = false); //Load a texture from a file. DDS and formats in the WIC are supported

	Texture2D(const DX::DeviceResourcesPtr& DeviceResources, const char* File, bool AllowWrites = false)
		: Texture2D(DeviceResources, std::string(File), AllowWrites) { } //Load a texture from a file. DDS and formats in the WIC are supported
	Texture2D(const DX::DeviceResourcesPtr& DeviceResources, const wchar_t* File, bool AllowWrites = false) //Load a texture from a file. DDS and formats in the WIC are supported
		: Texture2D(DeviceResources, std::wstring(File), AllowWrites) { }

	Texture2D& operator=(const Texture2D& Copy); //Copies shader resource view pointer (does not copy resource)
	Texture2D() : Texture(), desc({ 0 }) {  }

	//Set texture data (returns false if incorrect bounds)
	//Returns false on error
	template <class T>
	bool SetData(T* Data, const RECT& Bounds)
	{
		//make sure inside texture bounds
		if (!(Bounds.left < Bounds.right && Bounds.top < Bounds.bottom &&
			Bounds.left >= 0 && (unsigned)Bounds.right < desc.Width && Bounds.top >= 0 && (unsigned)Bounds.bottom < desc.Height))
			return false;

		ComPtr<ID3D11Resource> res;
		srv->GetResource(&res);

		//will overwrite old texture. requires USAGE_DYNAMIC & CPU_ACCESS_WRITE enabled. Note, this is faster than updating
		if ((desc.CPUAccessFlags & D3D11_CPU_ACCESS_WRITE) > 0 && desc.Usage == D3D11_USAGE_DYNAMIC)
		{
			D3D11_MAPPED_SUBRESOURCE map;
			HRESULT hr = deviceResources->GetD3DDeviceContext()->Map(res, 0, D3D11_MAP_WRITE_DISCARD, 0, &map);
			if (SUCCEEDED(hr))
			{
				unsigned wid = (Bounds.right - Bounds.left) * sizeof(T);
				for (unsigned y = 0; y < (Bounds.bottom - Bounds.top); y++)
					CopyMemory((void*)map.pData + y * map.RowPitch, (void*)Data + y * wid, wid);

				deviceResources->GetD3DDeviceContext()->Unmap(res, 0);
			}
			else
				return false;
		}
		else
		{
			//update a region in the texture; requires USAGE_DEFAULT and no CPU_ACCESS
			D3D11_BOX box = { Bounds.left, Bounds.top, 0, Bounds.right, Bounds.bottom, 1 };
			deviceResources->GetD3DDeviceContext()->UpdateSubresource(res, 0, &box, (void*)Data, (Bounds.right - Bounds.left) * sizeof(T), 0);
		}

		return true;
	}

	inline void Apply(unsigned Slot = 0) const { deviceResources->GetD3DDeviceContext()->PSSetShaderResources(Slot, 1, srv.GetAddressOf()); } //Apply this texture to the current pixel shader (Does not check if valid)

	inline POINT Size() const { POINT p = { desc.Width, desc.Height }; return p; }
	inline unsigned Width() const { return desc.Width; }
	inline unsigned Height() const { return desc.Height; }
	inline unsigned MipLevels() const { return desc.MipLevels; }
	inline DXGI_FORMAT Format() const { return desc.Format; }

	inline bool IsValid() const { return (srv != nullptr && deviceResources != nullptr); } //Is this texture valid?

protected:
	D3D11_TEXTURE2D_DESC desc;

	void LoadFromData(uint8_t* Data, unsigned Width, unsigned Height, bool AllowWrites = false); //Load the texture from data. Will update shader resource and desc (Assumes 32 bit pixels)
};