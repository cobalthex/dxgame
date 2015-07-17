#pragma once

#include "Pch.hpp"
#include "Engine/Graphics/Models/Model.hpp"
#include "Engine/Curves/Curve.hpp"

//The main map class for the racing game
class Map
{
public:
	std::shared_ptr<Model> world; //The static world should be one giant model
	std::vector<std::shared_ptr<Model>> models; //all other world objects (including animated/skinned models)

	Curve3d track;
	std::vector<float> checkpoints; //first is start, last is finish (percentage through curve (0 to 1))
};