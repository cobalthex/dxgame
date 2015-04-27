#pragma once

#include "Pch.hpp"
#include "Graphics/Models/StaticModel.hpp"
#include "Graphics/Textures/TextureCache.hpp"
#include "Graphics/Shaders/LitShader.hpp"

namespace Obj
{
	//Load a model from an OBJ file. Returns true if model is loaded - Stored into Model. If error, Model is not modified and returns false
	//Textures are stored in the texture cache and the lit skinned shader for which this model type uses
	bool Load(const DeviceResourcesPtr& DeviceResources, const std::string& Filename, TextureCache& TexCache, const std::shared_ptr<Shaders::LitShader>& Shader, __out StaticModel& Model);
};