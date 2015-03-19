#include "Pch.hpp"
#include "TextureCache.hpp"
#include "Common/Helpers.hpp"

std::shared_ptr<Texture2D>& TextureCache::Load(const std::string& Key)
{
	auto key = Key;
	StringToLower(key);

	if (Has(key))
		return cache[key];

	cache[key] = std::make_shared<Texture2D>(deviceResources, Key); //files may be case sensitive so use original key (POSIX)
	return cache[key];
}