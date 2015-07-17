#include "Pch.hpp"
#include "AppData.hpp"

namespace AppData
{
	std::string InstallFolder		= "";
	std::string BaseContentFolder	= "Content";

	std::string BaseModelsFolder	= "Models";
	std::string BaseMaterialsFolder = "Materials";
	std::string BaseTexturesFolder	= "Textures";
	std::string BaseAudioFolder		= "Audio";
	std::string BaseMapsFolder		= "Maps";
	std::string BaseShadersFolder	= "Shaders";
	std::string BaseFontsFolder		= "Fonts";

	std::wstring WInstallFolder			= StringOps::ToWString(AppData::InstallFolder);
	std::wstring WBaseContentFolder		= StringOps::ToWString(AppData::BaseContentFolder);

	std::wstring WBaseModelsFolder		= StringOps::ToWString(AppData::BaseModelsFolder);
	std::wstring WBaseMaterialsFolder	= StringOps::ToWString(AppData::BaseMaterialsFolder);
	std::wstring WBaseTexturesFolder	= StringOps::ToWString(AppData::BaseTexturesFolder);
	std::wstring WBaseAudioFolder		= StringOps::ToWString(AppData::BaseAudioFolder);
	std::wstring WBaseMapsFolder		= StringOps::ToWString(AppData::BaseMapsFolder);
	std::wstring WBaseShadersFolder		= StringOps::ToWString(AppData::BaseShadersFolder);
	std::wstring WBaseFontsFolder		= StringOps::ToWString(AppData::BaseFontsFolder);

	std::string LocalFolder		= "Local";
	std::string RoamingFolder	= "Roaming";

	std::wstring WLocalFolder	= StringOps::ToWString(AppData::LocalFolder);
	std::wstring WRoamingFolder = StringOps::ToWString(AppData::RoamingFolder);

	void CreateWStrings() //Create the wide string counterparts of the strings above
	{
		WInstallFolder			= StringOps::ToWString(AppData::InstallFolder);
		WBaseContentFolder		= StringOps::ToWString(AppData::BaseContentFolder);
		
		WBaseModelsFolder		= StringOps::ToWString(AppData::BaseModelsFolder);
		WBaseMaterialsFolder	= StringOps::ToWString(AppData::BaseMaterialsFolder);
		WBaseTexturesFolder		= StringOps::ToWString(AppData::BaseTexturesFolder);
		WBaseAudioFolder		= StringOps::ToWString(AppData::BaseAudioFolder);
		WBaseMapsFolder			= StringOps::ToWString(AppData::BaseMapsFolder);
		WBaseShadersFolder		= StringOps::ToWString(AppData::BaseShadersFolder);
		WBaseFontsFolder		= StringOps::ToWString(AppData::BaseFontsFolder);

		WLocalFolder	= StringOps::ToWString(AppData::LocalFolder);
		WRoamingFolder	= StringOps::ToWString(AppData::RoamingFolder);
	}
	void CreateStrings() //Create the wide string counterparts of the strings above
	{
		InstallFolder		= StringOps::ToString(AppData::WInstallFolder);
		BaseContentFolder	= StringOps::ToString(AppData::WBaseContentFolder);
														   
		BaseModelsFolder	= StringOps::ToString(AppData::WBaseModelsFolder);
		BaseMaterialsFolder	= StringOps::ToString(AppData::WBaseMaterialsFolder);
		BaseTexturesFolder	= StringOps::ToString(AppData::WBaseTexturesFolder);
		BaseAudioFolder		= StringOps::ToString(AppData::WBaseAudioFolder);
		BaseMapsFolder		= StringOps::ToString(AppData::WBaseMapsFolder);
		BaseShadersFolder	= StringOps::ToString(AppData::WBaseShadersFolder);
		BaseFontsFolder		= StringOps::ToString(AppData::WBaseFontsFolder);

		LocalFolder		= StringOps::ToString(AppData::WLocalFolder);
		RoamingFolder	= StringOps::ToString(AppData::WRoamingFolder);
	}
}