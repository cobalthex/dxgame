#pragma once

#include "Pch.hpp"
#include "Engine/Graphics/DeviceResources.hpp"
#include "Texture.hpp"

class Texture3D : public Texture
{
public:
	inline SIZE Size() const { SIZE p = { desc.Width, desc.Height }; return p; } //2D size of the textures
	inline unsigned Width() const { return desc.Width; }
	inline unsigned Height() const { return desc.Height; }
	inline unsigned Depth() const { return desc.Depth; }
	inline unsigned MipLevels() const { return desc.MipLevels; }
	inline DXGI_FORMAT Format() const { return desc.Format; }

protected:
	D3D11_TEXTURE3D_DESC desc;
};