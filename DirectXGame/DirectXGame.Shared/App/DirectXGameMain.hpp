#pragma once

#include "Common/StepTimer.hpp"
#include "Common/DeviceResources.hpp"
#include "Game/Sample3DSceneRenderer.hpp"
#include "Graphics/Text/SampleFpsTextRenderer.hpp"
#include "Graphics/Text/TextRenderer.hpp"

// Renders Direct2D and 3D content on the screen.
namespace DirectXGame
{
	class DirectXGameMain : public DX::IDeviceNotify
	{
	public:
		DirectXGameMain(const std::shared_ptr<DX::DeviceResources>& DeviceResources);
		~DirectXGameMain();
		void CreateWindowSizeDependentResources();
		void Update();
		bool Render();

		// IDeviceNotify
		virtual void OnDeviceLost();
		virtual void OnDeviceRestored();

	private:
		// Cached pointer to device resources.
		std::shared_ptr<DX::DeviceResources> deviceResources;

		// TODO: Replace with your own content renderers.
		std::unique_ptr<Sample3DSceneRenderer> sceneRenderer;
		std::unique_ptr<SampleFpsTextRenderer> fpsTextRenderer;
		std::unique_ptr<TextRenderer> textRenderer;
		TextLayoutParams tlParams;
		D2DBrush brush;

		// Rendering loop timer
		DX::StepTimer timer;
	};
}