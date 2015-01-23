#pragma once

#include "Common/DeviceResources.hpp"
#include "Graphics/ShaderStructures.hpp"
#include "Common/StepTimer.hpp"
#include "Graphics/Models/Model.hpp"

namespace DirectXGame
{
	// This sample renderer instantiates a basic rendering pipeline.
	class Sample3DSceneRenderer
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

		// Direct3D resources for cube geometry.
		ComPtr<ID3D11InputLayout>	inputLayout;
		ComPtr<ID3D11Buffer>		vertexBuffer;
		ComPtr<ID3D11Buffer>		indexBuffer;
		ComPtr<ID3D11VertexShader>	vertexShader;
		ComPtr<ID3D11PixelShader>	pixelShader;
		ComPtr<ID3D11Buffer>		constantBuffer;


		Model iqm;

		// System resources for cube geometry.
		ModelViewProjectionConstantBuffer constantBufferData;
		uint32_t indexCount;

		// Variables used with the rendering loop.
		bool	loadingComplete;
		float	degreesPerSecond;
		bool	tracking;
	};
}

