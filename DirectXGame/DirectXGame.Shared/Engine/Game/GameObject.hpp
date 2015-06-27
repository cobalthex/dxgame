#pragma once

#include "Pch.hpp"
#include "Engine/Graphics/Models/Model.hpp"

//A single object in the game
class GameObject
{
public:
	std::shared_ptr<Model> model; //the object's model

	Vector3 position; //position in the world

	bool isAlive; //is this object alive to be updated and drawn
};