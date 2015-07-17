#pragma once

#include "Pch.hpp"
#include "Engine/Common/StringOps.hpp"

//A collection of all of the sytem settings for this game
namespace AppData
{
	extern std::string InstallFolder; //Where the app is installed (and where assets are stored)
	extern std::string BaseContentFolder; //Where in the install folder all content is stored

	extern std::string BaseModelsFolder;
	extern std::string BaseMaterialsFolder;
	extern std::string BaseTexturesFolder;
	extern std::string BaseAudioFolder;
	extern std::string BaseMapsFolder;
	extern std::string BaseShadersFolder;
	extern std::string BaseFontsFolder;

	extern std::wstring WInstallFolder;
	extern std::wstring WBaseContentFolder;

	extern std::wstring WBaseModelsFolder;
	extern std::wstring WBaseMaterialsFolder;
	extern std::wstring WBaseTexturesFolder;
	extern std::wstring WBaseAudioFolder;
	extern std::wstring WBaseMapsFolder;
	extern std::wstring WBaseShadersFolder;
	extern std::wstring WBaseFontsFolder;

	inline std::string GetModelFile(const std::string& ModelName) { return StringOps::CombinePaths(InstallFolder, BaseContentFolder, BaseModelsFolder, ModelName); }
	inline std::string GetMaterialFile(const std::string& MaterialName) { return StringOps::CombinePaths(InstallFolder, BaseContentFolder, BaseMaterialsFolder, MaterialName); }
	inline std::string GetTextureFile(const std::string& TextureName) { return StringOps::CombinePaths(InstallFolder, BaseContentFolder, BaseTexturesFolder, TextureName); }
	inline std::string GetAudioFile(const std::string& AudioName) { return StringOps::CombinePaths(InstallFolder, BaseContentFolder, BaseAudioFolder, AudioName); }
	inline std::string GetMapFile(const std::string& MapName) { return StringOps::CombinePaths(InstallFolder, BaseContentFolder, BaseMapsFolder, MapName); }
	inline std::string GetShaderFile(const std::string& ShaderName) { return StringOps::CombinePaths(InstallFolder, BaseShadersFolder, ShaderName); } //Shaders are stored separately
	inline std::string GetFontFile(const std::string& FontName) { return StringOps::CombinePaths(InstallFolder, BaseContentFolder, BaseFontsFolder, FontName); }

	inline std::wstring GetModelFile(const std::wstring& ModelName) { return StringOps::CombinePaths(WInstallFolder, WBaseContentFolder, WBaseModelsFolder, ModelName); }
	inline std::wstring GetMaterialFile(const std::wstring& MaterialName) { return StringOps::CombinePaths(WInstallFolder, WBaseContentFolder, WBaseMaterialsFolder, MaterialName); }
	inline std::wstring GetTextureFile(const std::wstring& TextureName) { return StringOps::CombinePaths(WInstallFolder, WBaseContentFolder, WBaseTexturesFolder, TextureName); }
	inline std::wstring GetAudioFile(const std::wstring& AudioName) { return StringOps::CombinePaths(WInstallFolder, WBaseContentFolder, WBaseAudioFolder, AudioName); }
	inline std::wstring GetMapFile(const std::wstring& MapName) { return StringOps::CombinePaths(WInstallFolder, WBaseContentFolder, WBaseMapsFolder, MapName); }
	inline std::wstring GetShaderFile(const std::wstring& ShaderName) { return StringOps::CombinePaths(WInstallFolder, WBaseShadersFolder, ShaderName); }
	inline std::wstring GetFontFile(const std::wstring& FontName) { return StringOps::CombinePaths(WInstallFolder, WBaseContentFolder, WBaseFontsFolder, FontName); }

	extern std::string LocalFolder; //The folder where app generated (local only) data should be stored
	extern std::string RoamingFolder; //The folder where any user settings and data should be stored

	extern std::wstring WLocalFolder;
	extern std::wstring WRoamingFolder;

	extern void CreateWStrings(); //Create the wide string counterparts of the strings above
	extern void CreateStrings(); //Create the string counterparts of the wide strings above
}