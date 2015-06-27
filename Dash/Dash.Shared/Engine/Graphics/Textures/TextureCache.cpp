#include "Pch.hpp"
#include "TextureCache.hpp"
#include "App/SystemSettings.hpp"
#include "Engine/Common/PlatformHelpers.hpp"

std::shared_ptr<Texture2D> TextureCache::Load(const std::string& Key)
{
	if (Key.length() < 1)
		return nullptr;

	auto key = StringOps::ToLower(Key);

	if (Has(key))
		return cache[key];

	auto file = key;
	if (file.front() == '~')
		file = file.substr(1);
	if (file.front() != '/')
		file = SystemSettings::GetTextureFile(file);

	if (Sys::FileExists(file))
	{
		cache[key] = std::make_shared<Texture2D>(deviceResources, file); //files may be case sensitive so use original key (POSIX)
		return cache[key];
	}
	return nullptr;
}

bool TextureCache::Reload()
{
	bool allGood = true;
	for (auto& t : cache)
	{
		if (Sys::FileExists(t.first))
			t.second = std::make_shared<Texture2D>(deviceResources, t.first);
		else
			allGood = false;
	}
	return allGood;
}