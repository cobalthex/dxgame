#pragma once

#include "Pch.hpp"
#include "iqm.hpp"
#include "Engine/Graphics/Models/SkinnedModel.hpp"
#include "Engine/Graphics/Textures/TextureCache.hpp"
#include "Engine/Graphics/Shaders/ShaderCache.hpp"

namespace Iqm
{
	//Load a model from an IQM file
	//Textures are stored in the texture cache and shaders in the shader cache
	//A pointer to the model is returned or null on failure
	//Will return either a static or animated model
	std::shared_ptr<Model> Load(const DeviceResourcesPtr& DeviceResources, const std::string& Filename, TextureCache& TexCache);

	extern const float DefaultFrameTime; //Default length of each frame (in milliseconds) if none are specified in IQM file (defaults to ~24fps)
};