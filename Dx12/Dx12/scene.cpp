#include "pch.hpp"
#include "Scene.hpp"

#include "Graphics\DirectXHelper.hpp"
#include <ppltasks.h>
#include <synchapi.h>

using namespace Dx12;

using namespace Concurrency;
using namespace DirectX;
using namespace Microsoft::WRL;
using namespace Windows::Foundation;
using namespace Windows::Storage;

// Indices into the application state map.
Platform::String^ AngleKey = "Angle";
Platform::String^ TrackingKey = "Tracking";

// Loads vertex and pixel shaders from files and instantiates the cube geometry.
Scene::Scene(const std::shared_ptr<DX::DeviceResources>& deviceResources) :
	loadingComplete(false),
	radiansPerSecond(XM_PIDIV4),	// rotate 45 degrees per second
	angle(0),
	tracking(false),
	mappedConstantBuffer(nullptr),
	deviceResources(deviceResources)
{
	LoadState();
	ZeroMemory(&constantBufferData, sizeof(constantBufferData));

	CreateDeviceDependentResources();
	CreateWindowSizeDependentResources();
}

Scene::~Scene()
{
	constantBuffer->Unmap(0, nullptr);
	mappedConstantBuffer = nullptr;
}

void Scene::CreateDeviceDependentResources()
{
	auto d3dDevice = deviceResources->GetD3DDevice();

	// Create a root signature with a single constant buffer slot.
	{
		CD3DX12_DESCRIPTOR_RANGE range;
		CD3DX12_ROOT_PARAMETER parameter;

		range.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);
		parameter.InitAsDescriptorTable(1, &range, D3D12_SHADER_VISIBILITY_VERTEX);

		D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | // Only the input assembler stage needs access to the constant buffer.
			D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;

		CD3DX12_ROOT_SIGNATURE_DESC descRootSignature;
		descRootSignature.Init(1, &parameter, 0, nullptr, rootSignatureFlags);

		ComPtr<ID3DBlob> pSignature;
		ComPtr<ID3DBlob> pError;
		DX::ThrowIfFailed(D3D12SerializeRootSignature(&descRootSignature, D3D_ROOT_SIGNATURE_VERSION_1, pSignature.GetAddressOf(), pError.GetAddressOf()));
		DX::ThrowIfFailed(d3dDevice->CreateRootSignature(0, pSignature->GetBufferPointer(), pSignature->GetBufferSize(), IID_PPV_ARGS(&rootSignature)));
	}

	// Load shaders asynchronously.
	auto createVSTask = DX::ReadDataAsync(L"shader.vs.cso").then([this](std::vector<byte>& fileData) {
		vertexShader = fileData;
	});

	auto createPSTask = DX::ReadDataAsync(L"shader.ps.cso").then([this](std::vector<byte>& fileData) {
		pixelShader = fileData;
	});

	// Create the pipeline state once the shaders are loaded.
	auto createPipelineStateTask = (createPSTask && createVSTask).then([this]() {

		static const D3D12_INPUT_ELEMENT_DESC inputLayout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "BLENDINDICES", 0, DXGI_FORMAT_R8G8B8A8_UINT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "BLENDWEIGHTS", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		};

		D3D12_GRAPHICS_PIPELINE_STATE_DESC state = {};
		state.InputLayout = { inputLayout, _countof(inputLayout) };
		state.pRootSignature = rootSignature.Get();
		state.VS = { &vertexShader[0], vertexShader.size() };
		state.PS = { &pixelShader[0], pixelShader.size() };
		state.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		state.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		state.DepthStencilState.DepthEnable = FALSE;
		state.DepthStencilState.StencilEnable = FALSE;
		state.SampleMask = UINT_MAX;
		state.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		state.NumRenderTargets = 1;
		state.RTVFormats[0] = DXGI_FORMAT_B8G8R8A8_UNORM;
		state.SampleDesc.Count = 1;

		DX::ThrowIfFailed(deviceResources->GetD3DDevice()->CreateGraphicsPipelineState(&state, IID_PPV_ARGS(&pipelineState)));

		// Shader data can be deleted once the pipeline state is created.
		vertexShader.clear();
		pixelShader.clear();
	});

	// Create and upload cube geometry resources to the GPU.
	auto createAssetsTask = createPipelineStateTask.then([this]() {
		auto d3dDevice = deviceResources->GetD3DDevice();

		// Create a command list.
		DX::ThrowIfFailed(d3dDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, deviceResources->GetCommandAllocator(), pipelineState.Get(), IID_PPV_ARGS(&commandList)));

		CD3DX12_HEAP_PROPERTIES uploadHeapProperties(D3D12_HEAP_TYPE_UPLOAD);

		// Create a descriptor heap for the constant buffers.
		{
			D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
			heapDesc.NumDescriptors = DX::c_frameCount;
			heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
			// This flag indicates that this descriptor heap can be bound to the pipeline and that descriptors contained in it can be referenced by a root table.
			heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
			DX::ThrowIfFailed(d3dDevice->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&cbvHeap)));

			cbvHeap->SetName(L"Constant Buffer View Descriptor Heap");
		}

		CD3DX12_RESOURCE_DESC constantBufferDesc = CD3DX12_RESOURCE_DESC::Buffer(DX::c_frameCount * c_alignedConstantBufferSize);
		DX::ThrowIfFailed(d3dDevice->CreateCommittedResource(
			&uploadHeapProperties,
			D3D12_HEAP_FLAG_NONE,
			&constantBufferDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&constantBuffer)));

		constantBuffer->SetName(L"Constant Buffer");

		// Create constant buffer views to access the upload buffer.
		D3D12_GPU_VIRTUAL_ADDRESS cbvGpuAddress = constantBuffer->GetGPUVirtualAddress();
		CD3DX12_CPU_DESCRIPTOR_HANDLE cbvCpuHandle(cbvHeap->GetCPUDescriptorHandleForHeapStart());
		cbvDescriptorSize = d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		for (int n = 0; n < DX::c_frameCount; n++)
		{
			D3D12_CONSTANT_BUFFER_VIEW_DESC desc;
			desc.BufferLocation = cbvGpuAddress;
			desc.SizeInBytes = c_alignedConstantBufferSize;
			d3dDevice->CreateConstantBufferView(&desc, cbvCpuHandle);

			cbvGpuAddress += desc.SizeInBytes;
			cbvCpuHandle.Offset(cbvDescriptorSize);
		}

		std::ifstream fin("Content/mrfixit.iqm", std::ios::binary | std::ios::in);
		assert(fin.is_open());
		model = Model(deviceResources, fin, commandList);
		fin.close();

		// Map the constant buffers.
		DX::ThrowIfFailed(constantBuffer->Map(0, nullptr, reinterpret_cast<void**>(&mappedConstantBuffer)));
		ZeroMemory(mappedConstantBuffer, DX::c_frameCount * c_alignedConstantBufferSize);
		// We don't unmap this until the app closes. Keeping things mapped for the lifetime of the resource is okay.

		// Close the command list and execute it to begin the vertex/index buffer copy into the GPU's default heap.
		DX::ThrowIfFailed(commandList->Close());
		ID3D12CommandList* ppCommandLists[] = { commandList.Get() };
		deviceResources->GetCommandQueue()->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

		// Create vertex/index buffer views.
		vertexBufferView.BufferLocation = model.vertexBuffer->GetGPUVirtualAddress();
		vertexBufferView.StrideInBytes = sizeof(Vertex);
		vertexBufferView.SizeInBytes = sizeof(Vertex) * model.vertexCount;

		indexBufferView.BufferLocation = model.indexBuffer->GetGPUVirtualAddress();
		indexBufferView.SizeInBytes = sizeof(unsigned) * model.indexCount;
		indexBufferView.Format = DXGI_FORMAT_R32_UINT;

		// Wait for the command list to finish executing; the vertex/index buffers need to be uploaded to the GPU before the upload resources go out of scope.
		deviceResources->WaitForGpu();
	});

	createAssetsTask.then([this]() {
		loadingComplete = true;
	});
}

// Initializes view parameters when the window size changes.
void Scene::CreateWindowSizeDependentResources()
{
	Size outputSize = deviceResources->GetOutputSize();
	float aspectRatio = outputSize.Width / outputSize.Height;
	float fovAngleY = 70.0f * XM_PI / 180.0f;

	D3D12_VIEWPORT viewport = deviceResources->GetScreenViewport();
	scissorRect = { 0, 0, static_cast<LONG>(viewport.Width), static_cast<LONG>(viewport.Height)};

	// This is a simple example of change that can be made when the app is in
	// portrait or snapped view.
	if (aspectRatio < 1.0f)
	{
		fovAngleY *= 2.0f;
	}

	// Note that the OrientationTransform3D matrix is post-multiplied here
	// in order to correctly orient the scene to match the display orientation.
	// This post-multiplication step is required for any draw calls that are
	// made to the swap chain render target. For draw calls to other targets,
	// this transform should not be applied.

	// This sample makes use of a right-handed coordinate system using row-major matrices.
	XMMATRIX perspectiveMatrix = XMMatrixPerspectiveFovRH(
		fovAngleY,
		aspectRatio,
		0.01f,
		100.0f
		);

	XMFLOAT4X4 orientation = deviceResources->GetOrientationTransform3D();
	XMMATRIX orientationMatrix = XMLoadFloat4x4(&orientation);

	
	projection = XMMatrixTranspose(perspectiveMatrix * orientationMatrix);

	// Eye is at (0,0.7,1.5), looking at point (0,-0.1,0) with the up-vector along the y-axis.
	static const XMVECTORF32 eye = { 0.0f, 6.0f, 10.0f, 0.0f };
	static const XMVECTORF32 at = { 0.0f, 3.0f, 0.0f, 0.0f };
	static const XMVECTORF32 up = { 0.0f, 1.0f, 0.0f, 0.0f };

	view = XMMatrixTranspose(XMMatrixLookAtRH(eye, at, up));
}

// Called once per frame, rotates the cube and calculates the model and view matrices.
void Scene::Update(DX::StepTimer const& timer)
{
	if (loadingComplete)
	{
		if (!tracking)
		{
			// Rotate the cube a small amount.
			angle += static_cast<float>(timer.GetElapsedSeconds()) * radiansPerSecond;

			Rotate(angle);
		}

		// Update the constant buffer resource.
		UINT8* destination = mappedConstantBuffer + (deviceResources->GetCurrentFrameIndex() * c_alignedConstantBufferSize);
		memcpy(destination, &constantBufferData, sizeof(constantBufferData));
	}
}

// Saves the current state of the renderer.
void Scene::SaveState()
{
	auto state = ApplicationData::Current->LocalSettings->Values;

	if (state->HasKey(AngleKey))
	{
		state->Remove(AngleKey);
	}
	if (state->HasKey(TrackingKey))
	{
		state->Remove(TrackingKey);
	}

	state->Insert(AngleKey, PropertyValue::CreateSingle(angle));
	state->Insert(TrackingKey, PropertyValue::CreateBoolean(tracking));
}

// Restores the previous state of the renderer.
void Scene::LoadState()
{
	auto state = ApplicationData::Current->LocalSettings->Values;
	if (state->HasKey(AngleKey))
	{
		angle = safe_cast<IPropertyValue^>(state->Lookup(AngleKey))->GetSingle();
		state->Remove(AngleKey);
	}
	if (state->HasKey(TrackingKey))
	{
		tracking = safe_cast<IPropertyValue^>(state->Lookup(TrackingKey))->GetBoolean();
		state->Remove(TrackingKey);
	}
}

// Rotate the 3D cube model a set amount of radians.
void Scene::Rotate(float radians)
{
	// Prepare to pass the updated model matrix to the shader.
	auto world = XMMatrixTranspose(XMMatrixRotationX(-XM_PIDIV2) * XMMatrixRotationY(radians));
	XMStoreFloat4x4(&constantBufferData.world, world);
	XMStoreFloat4x4(&constantBufferData.worldViewProjection, (projection * view) * world);
	XMStoreFloat4x4(&constantBufferData.inverseTransposeWorld, XMMatrixTranspose(XMMatrixInverse(nullptr, world)));
}

void Scene::StartTracking()
{
	tracking = true;
}

// When tracking, the 3D cube can be rotated around its Y axis by tracking pointer position relative to the output screen width.
void Scene::TrackingUpdate(float positionX)
{
	if (tracking)
	{
		float radians = XM_2PI * 2.0f * positionX / deviceResources->GetOutputSize().Width;
		Rotate(radians);
	}
}

void Scene::StopTracking()
{
	tracking = false;
}

// Renders one frame using the vertex and pixel shaders.
bool Scene::Render()
{
	// Loading is asynchronous. Only draw geometry after it's loaded.
	if (!loadingComplete)
	{
		return false;
	}

	DX::ThrowIfFailed(deviceResources->GetCommandAllocator()->Reset());

	// The command list can be reset anytime after ExecuteCommandList() is called.
	DX::ThrowIfFailed(commandList->Reset(deviceResources->GetCommandAllocator(), pipelineState.Get()));

	PIXBeginEvent(commandList.Get(), 0, L"Draw the cube");
	{
		// Set the graphics root signature and descriptor heaps to be used by this frame.
		commandList->SetGraphicsRootSignature(rootSignature.Get());
		ID3D12DescriptorHeap* ppHeaps[] = { cbvHeap.Get() };
		commandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
		
		// Bind the current frame's constant buffer to the pipeline.
		CD3DX12_GPU_DESCRIPTOR_HANDLE gpuHandle(cbvHeap->GetGPUDescriptorHandleForHeapStart(), deviceResources->GetCurrentFrameIndex(), cbvDescriptorSize);
		commandList->SetGraphicsRootDescriptorTable(0, gpuHandle);

		// Set the viewport and scissor rectangle.
		D3D12_VIEWPORT viewport = deviceResources->GetScreenViewport();
		commandList->RSSetViewports(1, &viewport);
		commandList->RSSetScissorRects(1, &scissorRect);

		// Indicate this resource will be in use as a render target.
		CD3DX12_RESOURCE_BARRIER renderTargetResourceBarrier =
			CD3DX12_RESOURCE_BARRIER::Transition(deviceResources->GetRenderTarget(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
		commandList->ResourceBarrier(1, &renderTargetResourceBarrier);

		// Record drawing commands.
		commandList->ClearRenderTargetView(deviceResources->GetRenderTargetView(), DirectX::Colors::CornflowerBlue, 0, nullptr);
		D3D12_CPU_DESCRIPTOR_HANDLE renderTargetView = deviceResources->GetRenderTargetView();
		commandList->OMSetRenderTargets(1, &renderTargetView, false, nullptr);

		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		commandList->IASetVertexBuffers(0, 1, &vertexBufferView);
		commandList->IASetIndexBuffer(&indexBufferView);
		commandList->DrawIndexedInstanced(model.indexCount, 1, 0, 0, 0);

		// Indicate that the render target will now be used to present when the command list is done executing.
		CD3DX12_RESOURCE_BARRIER presentResourceBarrier =
			CD3DX12_RESOURCE_BARRIER::Transition(deviceResources->GetRenderTarget(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
		commandList->ResourceBarrier(1, &presentResourceBarrier);
	}
	PIXEndEvent(commandList.Get());

	DX::ThrowIfFailed(commandList->Close());

	// Execute the command list.
	ID3D12CommandList* ppCommandLists[] = { commandList.Get() };
	deviceResources->GetCommandQueue()->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

	return true;
}
