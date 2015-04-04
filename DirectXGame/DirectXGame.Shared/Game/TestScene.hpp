#pragma once

#include "Graphics/Renderer.hpp"
#include "Graphics/Models/Model.hpp"
#include "Graphics/Models/StaticModel.hpp"
#include "Graphics/Textures/TextureCache.hpp"
#include "Graphics/Textures/Texture2D.hpp"
#include "Graphics/ConstantBuffer.hpp"
#include "Graphics/Lighting.hpp"
#include "Graphics/Scene/Camera.hpp"
#include "Common/AlignedStorage.hpp"
#include "Animation/Timeline.hpp"
#include "Graphics/Shaders/ShaderCache.hpp"

namespace DirectXGame
{
	// This sample renderer instantiates a basic rendering pipeline.
	class TestScene : public AlignedStorage<16>, public Renderer
	{
	public:
		TestScene(const DeviceResourcesPtr& DeviceResources);
		void CreateDeviceDependentResources();
		void CreateWindowSizeDependentResources();
		void ReleaseDeviceDependentResources();
		void Update(const StepTimer& Timer);
		void Render();
		void StartTracking();
		void TrackingUpdate(float PositionX);
		void StopTracking();
		bool IsTracking() { return tracking; }

	private:
		void Rotate(float Radians);

		TextureCache texCache;
		ShaderCache shCache;

		std::shared_ptr<Shaders::LitSkinnedShader> lsShader;
		std::shared_ptr<Shaders::PositionColorShader> pcShader;

		ComPtr<ID3D11InputLayout>	inputLayout;
		ComPtr<ID3D11SamplerState>	sampler;
		ComPtr<ID3D11RasterizerState> wireRasterizer;

		Camera cam;

		Model iqm;
		Mesh iqmSkel;
		Texture2D* tex;

		StaticModel stage;
		void CreateStage(float Radius = 10);

		Timeline timeline;

		// Variables used with the rendering loop.
		bool	loadingComplete;
		float	degreesPerSecond;
		bool	tracking;
	};
}