#pragma once

#include "Graphics\StepTimer.hpp"
#include "Graphics\DeviceResources.hpp"
#include "Scene.hpp"

// Renders Direct3D content on the screen.
namespace Dx12
{
	class Dx12Main
	{
	public:
		Dx12Main();
		void CreateRenderers(const std::shared_ptr<DX::DeviceResources>& deviceResources);
		void Update();
		bool Render();

		void OnWindowSizeChanged();
		void OnSuspending();
		void OnResuming();
		void OnDeviceRemoved();

	private:
		// TODO: Replace with your own content renderers.
		std::unique_ptr<Scene> sceneRenderer;

		// Rendering loop timer.
		DX::StepTimer timer;
	};
}