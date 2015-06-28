#include "Pch.hpp"
#include "AppData.hpp"

namespace AppData
{
	std::string BaseContentFolder	= "Content/";

	std::string BaseModelsFolder	= "Models/";
	std::string BaseMaterialsFolder = "Materials/";
	std::string BaseTexturesFolder	= "Textures/";
	std::string BaseAudioFolder		= "Audio/";
	std::string BaseMapsFolder		= "Maps/";
	std::string BaseShadersFolder	= "Shaders/";
	std::string BaseFontsFolder		= "Fonts/";

	std::wstring WBaseContentFolder		= StringOps::ToWString(AppData::BaseContentFolder);

	std::wstring WBaseModelsFolder		= StringOps::ToWString(AppData::BaseModelsFolder);
	std::wstring WBaseMaterialsFolder	= StringOps::ToWString(AppData::BaseMaterialsFolder);
	std::wstring WBaseTexturesFolder	= StringOps::ToWString(AppData::BaseTexturesFolder);
	std::wstring WBaseAudioFolder		= StringOps::ToWString(AppData::BaseAudioFolder);
	std::wstring WBaseMapsFolder		= StringOps::ToWString(AppData::BaseMapsFolder);
	std::wstring WBaseShadersFolder		= StringOps::ToWString(AppData::BaseShadersFolder);
	std::wstring WBaseFontsFolder		= StringOps::ToWString(AppData::BaseFontsFolder);
}