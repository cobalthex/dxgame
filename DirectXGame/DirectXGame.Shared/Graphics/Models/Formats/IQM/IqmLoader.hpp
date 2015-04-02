#pragma once

#include "Pch.hpp"
#include "iqm.hpp"
#include "Graphics/Models/Model.hpp"
#include "Graphics/Textures/TextureCache.hpp"
#include "Graphics/Shaders/LitSkinnedShader.hpp"

namespace Iqm
{
	//Load a model from an IQM file. Returns true if model is loaded - Stored into Model. If error, Model is not modified and returns false
	//Textures are stored in the texture cache and the lit skinned shader for which this model type uses
	bool Load(const DeviceResourcesPtr& DeviceResources, const std::string& Filename, TextureCache& TexCache, const std::shared_ptr<Shaders::LitSkinnedShader>& Shader, __out Model& Model);

	extern const float DefaultFrameLength; //Default length of each frame (in milliseconds) if none are specified in IQM file (defaults to ~24fps)
};