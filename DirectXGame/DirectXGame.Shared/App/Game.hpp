#pragma once

#include "Common/StepTimer.hpp"
#include "Graphics/DeviceResources.hpp"
#include "Game/TestScene.hpp"
#include "Graphics/Text/FpsRenderer.hpp"

//Renders Direct2D and 3D content on the screen.
namespace DirectXGame
{
	class Game : public IDeviceNotify
	{
	public:
		Game(const std::shared_ptr<DeviceResources>& DeviceResources);
		~Game();
		void CreateWindowResources();
		void Update();
		bool Render();

		//IDeviceNotify
		virtual void OnDeviceLost();
		virtual void OnDeviceRestored();

	private:
		//Cached pointer to device resources.
		DeviceResourcesPtr deviceResources;

		std::unique_ptr<TestScene> scene;
		std::unique_ptr<FpsRenderer> fpsTextRenderer;

		//Rendering loop timer
		StepTimer timer;
	};
}