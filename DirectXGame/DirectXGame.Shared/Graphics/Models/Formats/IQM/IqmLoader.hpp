#pragma once

#include "Pch.hpp"
#include "iqm.hpp"
#include "Graphics/Models/Model.hpp"

namespace Iqm
{
	//Load a model from an IQM file. Returns true if model is loaded - Stored into Model. If error, Model is not modified and returns false
	bool Load(const DX::DeviceResourcesPtr& DeviceResources, const std::string& Filename, __out Model& Model);
};