#include "Pch.hpp"
#include "TextureCache.hpp"
#include "App/SystemSettings.hpp"

std::shared_ptr<Texture2D> TextureCache::Load(const std::string& Key)
{
	if (Key.length() < 1)
		return nullptr;

	auto key = StringToLower(Key);

	if (Has(key))
		return cache[key];

	auto file = key;
	if (file.front() == '~')
		file = file.substr(1);
	if (file.front() != '/')
		file = SystemSettings::GetTextureFile(file);

	std::ifstream fin(file);
	if (fin.is_open())
	{
		cache[key] = std::make_shared<Texture2D>(deviceResources, file); //files may be case sensitive so use original key (POSIX)
		fin.close();
		return cache[key];
	}
	return nullptr;
}