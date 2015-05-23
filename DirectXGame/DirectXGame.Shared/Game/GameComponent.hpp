#pragma once

#include "Engine/Graphics/Drawable.hpp"
#include "Game.hpp"

//The base for game components (renderables mainly)
class GameComponent : public Drawable
{
public:
	GameComponent(Game& Game, const DeviceResourcesPtr& DeviceResources) : game(Game), Drawable(DeviceResources), isEnabled(true) { }
	virtual ~GameComponent() { }

	virtual void Update(const StepTimer& Timer) { } //Update

	bool isEnabled;

protected:
	Game& game;
};