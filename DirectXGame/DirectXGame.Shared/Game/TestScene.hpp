#pragma once

#include "App/GameComponent.hpp"
#include "Engine/Graphics/Models/Model.hpp"
#include "Engine/Graphics/Models/StaticModel.hpp"
#include "Engine/Graphics/Textures/TextureCache.hpp"
#include "Engine/Graphics/Textures/Texture2D.hpp"
#include "Engine/Graphics/Shaders/UnlitShader.hpp"
#include "Engine/Graphics/ConstantBuffer.hpp"
#include "Engine/Graphics/Lighting.hpp"
#include "Engine/Graphics/Scene/Camera.hpp"
#include "Engine/Common/AlignedStorage.hpp"
#include "Engine/Animation/Timeline.hpp"
#include "Engine/Graphics/Shaders/ShaderCache.hpp"

namespace DirectXGame
{
	// This sample renderer instantiates a basic rendering pipeline.
	class TestScene : public AlignedStorage<16>, public GameComponent
	{
	public:
		TestScene(Game& Game, const DeviceResourcesPtr& DeviceResources);
		void CreateDeviceResources();
		void CreateWindowResources(Windows::UI::Core::CoreWindow^ Window);
		void ReleaseDeviceResources();
		void Update(const StepTimer& Timer);
		void Render();
		bool IsTracking() { return tracking; }

	private:
		TextureCache texCache;

		std::shared_ptr<Shaders::LitSkinnedShader> lsShader;
		std::shared_ptr<Shaders::UnlitShader> ulShader;
		std::shared_ptr<Shaders::PositionColorShader> pcShader;

		ComPtr<ID3D11SamplerState>	sampler;
		ComPtr<ID3D11RasterizerState> wireRasterizer;

		Camera cam;
		Vector3 camRotation;

		Model iqm;
		Mesh iqmSkel;
		Texture2D* tex;

		StaticModel stage;
		void CreateStage(float Radius = 10);

		StaticModel sword;

		Timeline timeline;

		// Variables used with the rendering loop.
		bool	loadingComplete;
		float	degreesPerSecond;
		bool	tracking;


		bool isPointerPressed;
	};
}