#include "Pch.hpp"
#include "ShaderCache.hpp"
#include "StaticLitShader.hpp"
#include "StaticUnlitShader.hpp"

std::shared_ptr<Shader> ShaderCache::Load(const ShaderType& Key)
{
	if (Has(Key))
		return cache[Key];

	switch (Key)
	{
	case ShaderType::StaticLit: return (cache[Key] = std::make_shared<Shaders::StaticLitShader>(deviceResources));
	case ShaderType::StaticUnlit: return (cache[Key] = std::make_shared<Shaders::StaticUnlitShader>(deviceResources));
	default: return nullptr;
	}
}

bool ShaderCache::Reload()
{
	for (auto& s : cache)
	{
		switch (s.first)
		{
		case ShaderType::StaticLit: (s.second = std::make_shared<Shaders::StaticLitShader>(deviceResources));
		case ShaderType::StaticUnlit: (s.second = std::make_shared<Shaders::StaticUnlitShader>(deviceResources));
		}
	}

	return true;
}

std::shared_ptr<Shader> ShaderCache::ApplyShader(ShaderType Type)
{
	auto& sh = Load(Type);
	if (lastActiveShader != Type)
	{
		sh->Apply();
		lastActiveShader = Type;
	}
	return sh;
}
