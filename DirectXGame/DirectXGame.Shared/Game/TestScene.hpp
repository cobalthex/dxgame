#pragma once

#include "Graphics/Renderer.hpp"
#include "Graphics/Models/Model.hpp"
#include "Graphics/Textures/TextureCache.hpp"
#include "Graphics/Textures/Texture2D.hpp"
#include "Graphics/ConstantBuffer.hpp"
#include "Graphics/Lighting.hpp"
#include "Graphics/Scene/Camera.hpp"
#include "Common/AlignedStorage.hpp"
#include "Animation/Timeline.hpp"
#include "Graphics/Shaders/ShaderStructures.hpp"
#include "Graphics/Shaders/ShaderCache.hpp"
#include "Graphics/Shaders/PositionColorShader.hpp"

namespace DirectXGame
{
	// This sample renderer instantiates a basic rendering pipeline.
	class TestScene : public AlignedStorage<16>, public Renderer
	{
	public:
		TestScene(const DX::DeviceResourcesPtr& DeviceResources);
		void CreateDeviceDependentResources();
		void CreateWindowSizeDependentResources();
		void ReleaseDeviceDependentResources();
		void Update(const DX::StepTimer& Timer);
		void Render();
		void StartTracking();
		void TrackingUpdate(float PositionX);
		void StopTracking();
		bool IsTracking() { return tracking; }

	private:
		void Rotate(float Radians);

	private:
		VertexShader vshader;
		PixelShader pshader;

		TextureCache texCache;
		ShaderCache shCache;

		ComPtr<ID3D11InputLayout>	inputLayout;
		ComPtr<ID3D11SamplerState>	sampler;
		ComPtr<ID3D11RasterizerState> wireRasterizer;

		Shaders::PositionColorShader pcShader;

		Camera cam;
		
		ConstantBuffer<ObjectConstantBufferDef> objectCBuffer;
		ConstantBuffer<LightConstantBufferDef> lightingCBuffer;
		ConstantBuffer<MaterialConstantBufferDef> materialCBuffer;

		Model iqm;
		Mesh iqmSkel;
		Texture2D* tex;

		Timeline timeline;

		// Variables used with the rendering loop.
		bool	loadingComplete;
		float	degreesPerSecond;
		bool	tracking;
	};
}