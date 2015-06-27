#include "Pch.hpp"
#include "SystemSettings.hpp"

namespace SystemSettings
{
	std::string BaseContentFolder	= "Content/";

	std::string BaseModelsFolder	= "Models/";
	std::string BaseMaterialsFolder = "Materials/";
	std::string BaseTexturesFolder	= "Textures/";
	std::string BaseAudioFolder		= "Audio/";
	std::string BaseMapsFolder		= "Maps/";
	std::string BaseShadersFolder	= "Shaders/";
	std::string BaseFontsFolder		= "Fonts/";

	std::wstring WBaseContentFolder		= StringOps::ToWString(SystemSettings::BaseContentFolder);

	std::wstring WBaseModelsFolder		= StringOps::ToWString(SystemSettings::BaseModelsFolder);
	std::wstring WBaseMaterialsFolder	= StringOps::ToWString(SystemSettings::BaseMaterialsFolder);
	std::wstring WBaseTexturesFolder	= StringOps::ToWString(SystemSettings::BaseTexturesFolder);
	std::wstring WBaseAudioFolder		= StringOps::ToWString(SystemSettings::BaseAudioFolder);
	std::wstring WBaseMapsFolder		= StringOps::ToWString(SystemSettings::BaseMapsFolder);
	std::wstring WBaseShadersFolder		= StringOps::ToWString(SystemSettings::BaseShadersFolder);
	std::wstring WBaseFontsFolder		= StringOps::ToWString(SystemSettings::BaseFontsFolder);
}