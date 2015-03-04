#pragma once

#include "Graphics/Renderer.hpp"
#include "Graphics/ShaderStructures.hpp"
#include "Graphics/Models/Model.hpp"
#include "Graphics/Textures/Texture2D.hpp"
#include "Common/ContentCache.hpp"
#include "Graphics/ConstantBuffer.hpp"
#include "Graphics/Lighting.hpp"
#include "Graphics/Scene/Camera.hpp"
#include "Common/AlignedStorage.hpp"
#include "Animation/Timeline.hpp"

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
		//A content cache for storing textures and such
		ContentCache ccache;

		// Direct3D resources for cube geometry.
		ComPtr<ID3D11InputLayout>	inputLayout;
		ComPtr<ID3D11VertexShader>	vertexShader;
		ComPtr<ID3D11PixelShader>	pixelShader;
		ComPtr<ID3D11SamplerState>	sampler;
		ComPtr<ID3D11RasterizerState> wireRasterizer;

		//Position color shaders
		ComPtr<ID3D11VertexShader>	pcVertexShader;
		ComPtr<ID3D11PixelShader>	pcPixelShader;
		ComPtr<ID3D11InputLayout>	pcInputLayout;
		ConstantBuffer<WVPConstantBufferDef> pcCbuffer;

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