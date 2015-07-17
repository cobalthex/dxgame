#include "Pch.hpp"
#include "ModelCache.hpp"
#include "App/AppData.hpp"
#include "Engine/Common/PlatformHelpers.hpp"
#include "Engine/Graphics/Models/Formats/IQM/IqmLoader.hpp"
#include "Engine/Graphics/Models/Formats/OBJ/ObjLoader.hpp"

std::shared_ptr<Model> ModelCache::Load(const std::string& Key)
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
		file = AppData::GetModelFile(file);

	if (Sys::FileExists(file))
	{
		if (StringOps::EndsWith(StringOps::ToLower(file), ".iqm"))
			cache[key] = Iqm::Load(deviceResources, file, *texCache, *shCache);

		return cache[key];
	}
	return nullptr;
}

bool ModelCache::Reload()
{
	bool allGood = true;
	for (auto& t : cache)
	{
		if (Sys::FileExists(t.first))
			t.second = nullptr;
		else
			allGood = false;
	}
	return allGood;
}
