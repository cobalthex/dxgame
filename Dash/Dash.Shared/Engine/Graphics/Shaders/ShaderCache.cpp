#include "Pch.hpp"
#include "ShaderCache.hpp"

#include "PositionColorShader.hpp"
#include "PositionTextureShader.hpp"
#include "LitShader.hpp"
#include "UnlitShader.hpp"
#include "LitSkinnedShader.hpp"

Shader* Shader::ActiveShader = nullptr;

std::shared_ptr<Shader> ShaderCache::Load(const ShaderType& Key)
{
	if (Has(Key))
		return cache[Key];

	switch (Key)
	{
	case ShaderType::PositionColor: return (cache[Key] = std::make_shared<Shaders::PositionColorShader>(deviceResources));
	case ShaderType::PositionTexture: return (cache[Key] = std::make_shared<Shaders::PositionTextureShader>(deviceResources));
	case ShaderType::Lit: return (cache[Key] = std::make_shared<Shaders::LitShader>(deviceResources));
	case ShaderType::Unlit: return (cache[Key] = std::make_shared<Shaders::UnlitShader>(deviceResources));
	case ShaderType::LitSkinned: return (cache[Key] = std::make_shared<Shaders::LitSkinnedShader>(deviceResources));
	default: return nullptr;
	}
}

bool ShaderCache::Reload()
{
	for (auto& s : cache)
	{
		switch (s.first)
		{
		case ShaderType::PositionColor: (s.second = std::make_shared<Shaders::PositionColorShader>(deviceResources)); break;
		case ShaderType::PositionTexture: (s.second = std::make_shared<Shaders::PositionTextureShader>(deviceResources)); break;
		case ShaderType::Lit: (s.second = std::make_shared<Shaders::LitShader>(deviceResources)); break;
		case ShaderType::Unlit: (s.second = std::make_shared<Shaders::UnlitShader>(deviceResources)); break;
		case ShaderType::LitSkinned: (s.second = std::make_shared<Shaders::LitSkinnedShader>(deviceResources)); break;
		}
	}

	return true;
}