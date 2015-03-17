#include "Pch.hpp"
#include "TextureCache.hpp"

std::shared_ptr<Texture2D>& TextureCache::Load(const std::string& Key)
{
	if (Has(Key))
		return cache[Key];

	cache[Key] = std::make_shared<Texture2D>(deviceResources, Key);
	return cache[Key];
}