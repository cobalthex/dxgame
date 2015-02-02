﻿#include "Pch.hpp"
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
		fovAngleY *= 2.0f;

	objectCBuffer = ConstantBuffer<ObjectConstantBufferDef>(deviceResources);

	materialCBuffer = ConstantBuffer<MaterialConstantBufferDef>(deviceResources);

	D3D11_SAMPLER_DESC sampDesc;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = -FLT_MAX;
	sampDesc.MaxLOD = FLT_MAX;
	sampDesc.MaxAnisotropy = 16;
	sampDesc.MipLODBias = 0;
	deviceResources->GetD3DDevice()->CreateSamplerState(&sampDesc, &sampler);

	cam.orientation = deviceResources->GetOrientationTransform3D();

	cam.fieldOfView = fovAngleY;
	cam.aspectRatio = aspectRatio;
	cam.nearPlaneDistance = 0.01f;
	cam.farPlaneDistance = 1000;

	cam.position = { 0, 8, 8 };
	cam.lookAt = { 0, 3, 0 };

	cam.CalcMatrices();

	Light li;
	li.color = Color(1, 1, 0, 1);
	li.spotAngle = XMConvertToRadians(45);
	li.constantAttenuation = 1;
	li.linearAttenuation = 0.08f;
	li.quadraticAttenuation = 0;
	li.position = Vector4(1, 1, 0, 0);
	li.direction = -li.position;
	li.isEnabled = true;
	li.type = LightType::Directional;
	
	lightingCBuffer = ConstantBuffer<LightConstantBufferDef>(deviceResources);
	lightingCBuffer.data.lights[0] = li;
	lightingCBuffer.data.eyePosition = Vector4(cam.position);
	lightingCBuffer.data.globalAmbience = Color(0, 0, 0, 0);
	lightingCBuffer.Update();
}

void Sample3DSceneRenderer::CreateDeviceDependentResources()
{
	// Load shaders asynchronously.
	auto loadVSTask = DX::ReadDataAsync(L"Lit.vs.cso");
	auto loadPSTask = DX::ReadDataAsync(L"Lit.ps.cso");

	// After the vertex shader file is loaded, create the shader and input layout.
	auto createVSTask = loadVSTask.then([this](const std::vector<byte>& fileData)
	{
		DX::ThrowIfFailed(
			deviceResources->GetD3DDevice()->CreateVertexShader(
			fileData.data(),
			fileData.size(),
			nullptr,
			&vertexShader
			)
			);

		DX::ThrowIfFailed(
			deviceResources->GetD3DDevice()->CreateInputLayout(
			DirectXGame::VertexSkinned::ElementDesc,
			DirectXGame::VertexSkinned::ElementCount,
			fileData.data(),
			fileData.size(),
			&inputLayout
			)
			);
	});

	// After the pixel shader file is loaded, create the shader and constant buffer.
	auto createPSTask = loadPSTask.then([this](const std::vector<byte>& fileData)
	{
		DX::ThrowIfFailed(
			deviceResources->GetD3DDevice()->CreatePixelShader(
			&fileData[0],
			fileData.size(),
			nullptr,
			&pixelShader
			)
			);
	});

	// Once both shaders are loaded, create the mesh.
	auto loadModelTask = (createPSTask && createVSTask).then([this]()
	{
		Iqm::Load(deviceResources, ccache, "Content/test.iqm", iqm);

	}).then([this]()
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
	objectCBuffer.data.world = Matrix::CreateRotationY(Radians).Transpose();
	objectCBuffer.data.Calc(cam.View(), cam.Projection());
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
		return;

	auto context = deviceResources->GetD3DDeviceContext();

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	context->IASetInputLayout(inputLayout.Get());

	// Attach our vertex shader.
	context->VSSetShader(vertexShader.Get(), nullptr, 0);

	objectCBuffer.Update();
	objectCBuffer.BindVertex(0);

	// Attach our pixel shader.
	context->PSSetShader(pixelShader.Get(), nullptr, 0);
	context->PSSetSamplers(0, 1, sampler.GetAddressOf());

	lightingCBuffer.BindPixel(1);

	materialCBuffer.BindPixel(0);
	iqm.Bind();
	const auto& cxt = deviceResources->GetD3DDeviceContext();
	for (auto& mesh : iqm.meshes)
	{
		materialCBuffer.data.FillFromMaterial(mesh.material);
		materialCBuffer.Update();

		mesh.material.texture->Apply();
		cxt->DrawIndexed(mesh.IndexCount(), mesh.StartIndex(), 0);
	}
	//iqm.Draw();
}