#pragma once

#include "Pch.hpp"
#include "Engine/Graphics/Models/Model.hpp"
#include "Engine/Graphics/Textures/TextureCache.hpp"

namespace Obj
{
	//Load a model from an OBJ file. Returns true if model is loaded - Stored into Model. If error, Model is not modified and returns false
	//Textures are stored in the texture cache and the lit skinned shader for which this model type uses
	bool Load(const DeviceResourcesPtr& DeviceResources, const std::string& Filename, TextureCache& TexCache, __out Model& Model);
};