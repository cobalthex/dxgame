#pragma once

#include "Common/StepTimer.hpp"
#include "Graphics/DeviceResources.hpp"
#include "Game/TestScene.hpp"
#include "Graphics/Text/FpsRenderer.hpp"

//Renders Direct2D and 3D content on the screen.
namespace DirectXGame
{
	class Game : public DX::IDeviceNotify
	{
	public:
		Game(const std::shared_ptr<DX::DeviceResources>& DeviceResources);
		~Game();
		void CreateWindowSizeDependentResources();
		void Update();
		bool Render();

		//IDeviceNotify
		virtual void OnDeviceLost();
		virtual void OnDeviceRestored();

	private:
		//Cached pointer to device resources.
		DX::DeviceResourcesPtr deviceResources;

		std::unique_ptr<TestScene> scene;
		std::unique_ptr<FpsRenderer> fpsTextRenderer;

		//Rendering loop timer
		DX::StepTimer timer;
	};
}