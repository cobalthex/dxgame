#pragma once

#include "Common/DeviceResources.hpp"
#include "Graphics/ShaderStructures.hpp"
#include "Common/StepTimer.hpp"
#include "Graphics/Models/Model.hpp"
#include "Graphics/Textures/Texture2D.hpp"
#include "Common/ContentCache.hpp"
#include "Graphics/ConstantBuffer.hpp"
#include "Graphics/Lighting.hpp"
#include "Graphics/Scene/Camera.hpp"
#include "Common/AlignedStorage.hpp"

namespace DirectXGame
{
	// This sample renderer instantiates a basic rendering pipeline.
	class Sample3DSceneRenderer : public AlignedStorage<16>
	{
	public:
		Sample3DSceneRenderer(const DX::DeviceResourcesPtr& DeviceResources);
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
		// Cached pointer to device resources.
		DX::DeviceResourcesPtr deviceResources;

		//A content cache for storing textures and such
		ContentCache ccache;

		// Direct3D resources for cube geometry.
		ComPtr<ID3D11InputLayout>	inputLayout;
		ComPtr<ID3D11VertexShader>	vertexShader;
		ComPtr<ID3D11PixelShader>	pixelShader;
		ComPtr<ID3D11SamplerState>	sampler;

		Camera cam;
		
		ConstantBuffer<ObjectConstantBufferDef> objectCBuffer;
		ConstantBuffer<LightConstantBufferDef> lightingCBuffer;
		ConstantBuffer<MaterialConstantBufferDef> materialCBuffer;

		Model iqm;
		Texture2D* tex;

		// Variables used with the rendering loop.
		bool	loadingComplete;
		float	degreesPerSecond;
		bool	tracking;
	};
}