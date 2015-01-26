#include "Pch.hpp"
#include "Sample3DSceneRenderer.hpp"
#include "Common/PlatformHelpers.hpp"
#include "Graphics/Models/Formats/IQM/IqmLoader.hpp"

using namespace DirectXGame;

using namespace DirectX;
using namespace Windows::Foundation;

// Loads vertex and pixel shaders from files and instantiates the cube geometry.
Sample3DSceneRenderer::Sample3DSceneRenderer(const std::shared_ptr<DX::DeviceResources>& DeviceResources) :
	loadingComplete(false),
	degreesPerSecond(45),
	tracking(false),
	deviceResources(DeviceResources)
{
	CreateDeviceDependentResources();
	CreateWindowSizeDependentResources();
}

// Initializes view parameters when the window size changes.
void Sample3DSceneRenderer::CreateWindowSizeDependentResources()
{
	Size outputSize = deviceResources->GetOutputSize();
	float aspectRatio = outputSize.Width / outputSize.Height;
	float fovAngleY = 70.0f * XM_PI / 180.0f;

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

	XMStoreFloat4x4(
		&constantBufferData.projection,
		XMMatrixTranspose(perspectiveMatrix * orientationMatrix)
		);

	static const XMVECTORF32 eye = { 0.0f, 5.0f, 8.5f, 0.0f };
	static const XMVECTORF32 at = { 0.0f, 3.0f, 0.0f, 0.0f };
	static const XMVECTORF32 up = { 0.0f, 1.0f, 0.0f, 0.0f };

	XMStoreFloat4x4(&constantBufferData.view, XMMatrixTranspose(XMMatrixLookAtRH(eye, at, up)));
}

// Called once per frame, rotates the cube and calculates the model and view matrices.
void Sample3DSceneRenderer::Update(const DX::StepTimer& Timer)
{
	if (!tracking)
	{
		// Convert degrees to radians, then convert seconds to rotation angle
		float radiansPerSecond = XMConvertToRadians(degreesPerSecond);
		double totalRotation = Timer.GetTotalSeconds() * radiansPerSecond;
		float radians = static_cast<float>(fmod(totalRotation, XM_2PI));

		Rotate(radians);
	}
}

// Rotate the 3D cube model a set amount of radians.
void Sample3DSceneRenderer::Rotate(float Radians)
{
	// Prepare to pass the updated model matrix to the shader
	XMStoreFloat4x4(&constantBufferData.model, XMMatrixTranspose(XMMatrixRotationY(Radians)));
}

void Sample3DSceneRenderer::StartTracking()
{
	tracking = true;
}

// When tracking, the 3D cube can be rotated around its Y axis by tracking pointer position relative to the output screen width.
void Sample3DSceneRenderer::TrackingUpdate(float PositionX)
{
	if (tracking)
	{
		float radians = XM_2PI * 2.0f * PositionX / deviceResources->GetOutputSize().Width;
		Rotate(radians);
	}
}

void Sample3DSceneRenderer::StopTracking()
{
	tracking = false;
}

// Renders one frame using the vertex and pixel shaders.
void Sample3DSceneRenderer::Render()
{
	// Loading is asynchronous. Only draw geometry after it's loaded.
	if (!loadingComplete)
	{
		return;
	}

	auto context = deviceResources->GetD3DDeviceContext();

	// Prepare the constant buffer to send it to the graphics device.
	context->UpdateSubresource(
		constantBuffer.Get(),
		0,
		NULL,
		&constantBufferData,
		0,
		0
		);

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	context->IASetInputLayout(inputLayout.Get());

	// Attach our vertex shader.
	context->VSSetShader(
		vertexShader.Get(),
		nullptr,
		0
		);

	// Send the constant buffer to the graphics device.
	context->VSSetConstantBuffers(
		0,
		1,
		constantBuffer.GetAddressOf()
		);

	// Attach our pixel shader.
	context->PSSetShader(
		pixelShader.Get(),
		nullptr,
		0
		);

	iqm.Draw();
}

void Sample3DSceneRenderer::CreateDeviceDependentResources()
{
	// Load shaders asynchronously.
	auto loadVSTask = DX::ReadDataAsync(L"SampleVertexShader.cso");
	auto loadPSTask = DX::ReadDataAsync(L"SamplePixelShader.cso");

	// After the vertex shader file is loaded, create the shader and input layout.
	auto createVSTask = loadVSTask.then([this](const std::vector<byte>& fileData) {
		DX::ThrowIfFailed(
			deviceResources->GetD3DDevice()->CreateVertexShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				&vertexShader
				)
			);

		DX::ThrowIfFailed(
			deviceResources->GetD3DDevice()->CreateInputLayout(
				Model::VertexType::ElementDesc,
				ARRAYSIZE(Model::VertexType::ElementDesc),
				&fileData[0],
				fileData.size(),
				&inputLayout
				)
			);
	});

	// After the pixel shader file is loaded, create the shader and constant buffer.
	auto createPSTask = loadPSTask.then([this](const std::vector<byte>& fileData) {
		DX::ThrowIfFailed(
			deviceResources->GetD3DDevice()->CreatePixelShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				&pixelShader
				)
			);

		CD3D11_BUFFER_DESC constantBufferDesc(sizeof(ModelViewProjectionConstantBuffer) , D3D11_BIND_CONSTANT_BUFFER);
		DX::ThrowIfFailed(
			deviceResources->GetD3DDevice()->CreateBuffer(
				&constantBufferDesc,
				nullptr,
				&constantBuffer
				)
			);
	});

	// Once both shaders are loaded, create the mesh.
	auto loadModelTask = (createPSTask && createVSTask).then([this] ()
	{
		Iqm::Load(deviceResources, "Content/ship.iqm", iqm);
	}).then([this] ()
	{
		loadingComplete = true;
	});
}

void Sample3DSceneRenderer::ReleaseDeviceDependentResources()
{
	loadingComplete = false;
	vertexShader.Reset();
	inputLayout.Reset();
	pixelShader.Reset();
	constantBuffer.Reset();
}