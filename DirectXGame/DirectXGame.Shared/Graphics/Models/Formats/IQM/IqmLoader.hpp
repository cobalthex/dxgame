#pragma once

#include "Pch.hpp"
#include "iqm.hpp"
#include "Graphics/Models/Model.hpp"
#include "Graphics/Textures/TextureCache.hpp"

namespace Iqm
{
	//Load a model from an IQM file. Returns true if model is loaded - Stored into Model. If error, Model is not modified and returns false
	//Textures are stored in the content cache
	bool Load(const DeviceResourcesPtr& DeviceResources, TextureCache& TexCache, const std::string& Filename, __out Model& Model);

	extern const float DefaultFrameLength; //Default length of each frame (in milliseconds) if none are specified in IQM file (defaults to ~24fps)
};