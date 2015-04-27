#include "Pch.hpp"
#include "ShaderCache.hpp"

#include "PositionColorShader.hpp"
#include "PositionTextureShader.hpp"
#include "LitShader.hpp"
#include "LitSkinnedShader.hpp"

std::shared_ptr<Shader> ShaderCache::Load(const ShaderType& Key)
{
	if (Has(Key))
		return cache[Key];

	switch (Key)
	{
	case ShaderType::PositionColor: return (cache[Key] = std::make_shared<Shaders::PositionColorShader>(deviceResources));
	case ShaderType::PositionTexture: return (cache[Key] = std::make_shared<Shaders::PositionTextureShader>(deviceResources));
	case ShaderType::Lit: return (cache[Key] = std::make_shared<Shaders::LitShader>(deviceResources));
	case ShaderType::LitSkinned: return (cache[Key] = std::make_shared<Shaders::LitSkinnedShader>(deviceResources));
	default: return nullptr;
	}
}