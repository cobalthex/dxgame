#pragma once

#include "Common/StepTimer.hpp"
#include "Graphics/DeviceResources.hpp"
#include "Game.hpp"

//The base for game components (renderables mainly)
class GameComponent
{
public:
	GameComponent(Game& Game, const DeviceResourcesPtr& DeviceResources) : game(Game), deviceResources(DeviceResources), isEnabled(true), isVisible(true) { }
	virtual ~GameComponent() { }

	virtual void CreateDeviceResources() { } //Create any resources that depend on the graphics device
	virtual void CreateWindowResources(Windows::UI::Core::CoreWindow^ Window) { } //Create any resources that depend on the size of the window
	virtual void ReleaseDeviceResources() { } //Release any resources that depended on the graphics device
	virtual void Update(const StepTimer& Timer) { } //Update
	virtual void Render() { }; //Render

	bool isEnabled;
	bool isVisible;

protected:
	Game& game;
	DeviceResourcesPtr deviceResources;
};