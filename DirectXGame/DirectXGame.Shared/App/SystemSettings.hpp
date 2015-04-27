#pragma once

#include "Pch.hpp"
#include "Common/StringOps.hpp"

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

	inline std::string GetModelFile(const std::string& ModelName) { return StringOps::CombinePaths(BaseContentFolder, BaseModelsFolder, ModelName); }
	inline std::string GetMaterialFile(const std::string& ModelName) { return StringOps::CombinePaths(BaseContentFolder, BaseMaterialsFolder, ModelName); }
	inline std::string GetTextureFile(const std::string& ModelName) { return StringOps::CombinePaths(BaseContentFolder, BaseTexturesFolder, ModelName); }
	inline std::string GetAudioFile(const std::string& ModelName) { return StringOps::CombinePaths(BaseContentFolder, BaseAudioFolder, ModelName); }
	inline std::string GetMapFile(const std::string& ModelName) { return StringOps::CombinePaths(BaseContentFolder, BaseMapsFolder, ModelName); }
	inline std::string GetShaderFile(const std::string& ModelName) { return StringOps::CombinePaths(BaseContentFolder, BaseShadersFolder, ModelName); }
	inline std::string GetFontFile(const std::string& ModelName) { return StringOps::CombinePaths(BaseContentFolder, BaseFontsFolder, ModelName); }
}