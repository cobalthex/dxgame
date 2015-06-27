#pragma once

#include "Pch.hpp"
#include "Engine/Common/StringOps.hpp"

//A collection of all of the sytem settings for this game
namespace SystemSettings
{
	extern std::string BaseContentFolder;

	extern std::string BaseModelsFolder;
	extern std::string BaseMaterialsFolder;
	extern std::string BaseTexturesFolder;
	extern std::string BaseAudioFolder;
	extern std::string BaseMapsFolder;
	extern std::string BaseShadersFolder;
	extern std::string BaseFontsFolder;

	extern std::wstring WBaseContentFolder;

	extern std::wstring WBaseModelsFolder;
	extern std::wstring WBaseMaterialsFolder;
	extern std::wstring WBaseTexturesFolder;
	extern std::wstring WBaseAudioFolder;
	extern std::wstring WBaseMapsFolder;
	extern std::wstring WBaseShadersFolder;
	extern std::wstring WBaseFontsFolder;

	inline std::string GetModelFile(const std::string& ModelName) { return StringOps::CombinePaths(BaseContentFolder, BaseModelsFolder, ModelName); }
	inline std::string GetMaterialFile(const std::string& MaterialName) { return StringOps::CombinePaths(BaseContentFolder, BaseMaterialsFolder, MaterialName); }
	inline std::string GetTextureFile(const std::string& TextureName) { return StringOps::CombinePaths(BaseContentFolder, BaseTexturesFolder, TextureName); }
	inline std::string GetAudioFile(const std::string& AudioName) { return StringOps::CombinePaths(BaseContentFolder, BaseAudioFolder, AudioName); }
	inline std::string GetMapFile(const std::string& MapName) { return StringOps::CombinePaths(BaseContentFolder, BaseMapsFolder, MapName); }
	inline std::string GetShaderFile(const std::string& ShaderName) { return StringOps::CombinePaths(BaseContentFolder, BaseShadersFolder, ShaderName); }
	inline std::string GetFontFile(const std::string& FontName) { return StringOps::CombinePaths(BaseContentFolder, BaseFontsFolder, FontName); }

	inline std::wstring GetModelFile(const std::wstring& ModelName) { return StringOps::CombinePaths(WBaseContentFolder, WBaseModelsFolder, ModelName); }
	inline std::wstring GetMaterialFile(const std::wstring& MaterialName) { return StringOps::CombinePaths(WBaseContentFolder, WBaseMaterialsFolder, MaterialName); }
	inline std::wstring GetTextureFile(const std::wstring& TextureName) { return StringOps::CombinePaths(WBaseContentFolder, WBaseTexturesFolder, TextureName); }
	inline std::wstring GetAudioFile(const std::wstring& AudioName) { return StringOps::CombinePaths(WBaseContentFolder, WBaseAudioFolder, AudioName); }
	inline std::wstring GetMapFile(const std::wstring& MapName) { return StringOps::CombinePaths(WBaseContentFolder, WBaseMapsFolder, MapName); }
	inline std::wstring GetShaderFile(const std::wstring& ShaderName) { return StringOps::CombinePaths(WBaseContentFolder, WBaseShadersFolder, ShaderName); }
	inline std::wstring GetFontFile(const std::wstring& FontName) { return StringOps::CombinePaths(WBaseContentFolder, WBaseFontsFolder, FontName); }
}