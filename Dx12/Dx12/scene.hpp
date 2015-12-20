#pragma once

#include "Graphics\DeviceResources.hpp"
#include "Graphics\StepTimer.hpp"
#include "Graphics\ShaderStructures.hpp"
#include "Graphics\Model.hpp"

namespace Dx12
{
	// This sample renderer instantiates a basic rendering pipeline.
	class Scene
	{
	public:
		Scene(const DeviceResourcesPtr& deviceResources);
		~Scene();
		void CreateDeviceDependentResources();
		void CreateWindowSizeDependentResources();
		void Update(DX::StepTimer const& timer);
		bool Render();
		void SaveState();

		void StartTracking();
		void TrackingUpdate(float positionX);
		void StopTracking();
		bool IsTracking() { return tracking; }

	private:
		void LoadState();
		void Rotate(float radians);

	private:
		// Constant buffers must be 256-byte aligned.
		static const UINT c_alignedConstantBufferSize = (sizeof(ModelViewProjectionConstantBuffer) + 255) & ~255;

		// Cached pointer to device resources.
		DeviceResourcesPtr deviceResources;

		// Direct3D resources for cube geometry.
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>	commandList;
		Microsoft::WRL::ComPtr<ID3D12RootSignature>			rootSignature;
		Microsoft::WRL::ComPtr<ID3D12PipelineState>			pipelineState;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>		cbvHeap;
		Microsoft::WRL::ComPtr<ID3D12Resource>				vertexBuffer;
		Microsoft::WRL::ComPtr<ID3D12Resource>				indexBuffer;
		Microsoft::WRL::ComPtr<ID3D12Resource>				constantBuffer;
		ModelViewProjectionConstantBuffer					constantBufferData;
		UINT8*												mappedConstantBuffer;
		UINT												cbvDescriptorSize;
		D3D12_RECT											scissorRect;
		std::vector<byte>									vertexShader;
		std::vector<byte>									pixelShader;
		D3D12_VERTEX_BUFFER_VIEW							vertexBufferView;
		D3D12_INDEX_BUFFER_VIEW								indexBufferView;

		XMMATRIX view, projection;

		Model model;

		// Variables used with the rendering loop.
		bool	loadingComplete;
		float	radiansPerSecond;
		float	angle;
		bool	tracking;
	};
}

