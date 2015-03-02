#include "Pch.hpp"
#include "TestScene.hpp"
#include "Common/PlatformHelpers.hpp"
#include "Graphics/Models/Formats/IQM/IqmLoader.hpp"

using namespace DirectXGame;

using namespace DirectX;
using namespace Windows::Foundation;

//Loads vertex and pixel shaders from files and instantiates the cube geometry.
TestScene::TestScene(const std::shared_ptr<DX::DeviceResources>& DeviceResources) :
loadingComplete(false),
degreesPerSecond(45),
tracking(false),
Renderer(DeviceResources)
{
	CreateDeviceDependentResources();
	CreateWindowSizeDependentResources();
}

//Initializes view parameters when the window size changes.
void TestScene::CreateWindowSizeDependentResources()
{
	Size outputSize = deviceResources->GetOutputSize();
	float aspectRatio = outputSize.Width / outputSize.Height;
	float fovAngleY = 70.0f * XM_PI / 180.0f;

	//This is a simple example of change that can be made when the app is in
	//portrait or snapped view.
	if (aspectRatio < 1.0f)
		fovAngleY *= 2.0f;

	pcCbuffer = ConstantBuffer<WVPConstantBufferDef>(deviceResources);
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

	cam.position = { 0, 10, 10 };
	cam.lookAt = { 0, 3, 0 };

	cam.CalcMatrices();

	Light li;
	li.color = Color(1, 1, 1, 1);
	li.spotAngle = XMConvertToRadians(45);
	li.constantAttenuation = 1;
	li.linearAttenuation = 0.08f;
	li.quadraticAttenuation = 0;
	li.position = Vector4(5, 16, 13, 0);
	li.direction = -li.position;
	li.isEnabled = true;
	li.type = LightType::Point;

	lightingCBuffer = ConstantBuffer<LightConstantBufferDef>(deviceResources);
	lightingCBuffer.data.lights[0] = li;
	lightingCBuffer.data.eyePosition = Vector4(cam.position);
	lightingCBuffer.data.globalAmbience = Color(0, 0, 0, 0);
	lightingCBuffer.Update();

	ZeroMemory(objectCBuffer.data.joints, ARRAYSIZE(objectCBuffer.data.joints));

	D3D11_RASTERIZER_DESC rastDesc;
	ZeroMemory(&rastDesc, sizeof(D3D11_RASTERIZER_DESC));
	rastDesc.FillMode = D3D11_FILL_WIREFRAME;
	rastDesc.CullMode = D3D11_CULL_NONE;
	rastDesc.AntialiasedLineEnable = true;
	rastDesc.MultisampleEnable = true;
	App::ThrowIfFailed(deviceResources->GetD3DDevice()->CreateRasterizerState(&rastDesc, &wireRasterizer));
}

void TestScene::CreateDeviceDependentResources()
{
	//Load shaders asynchronously.
	auto loadVSTask = App::ReadDataAsync(L"LitSkinned.vs.cso");
	auto loadPSTask = App::ReadDataAsync(L"LitSkinned.ps.cso");

	//After the vertex shader file is loaded, create the shader and input layout.
	auto createVSTask = loadVSTask.then([this](const std::vector<byte>& fileData)
	{
		App::ThrowIfFailed(
			deviceResources->GetD3DDevice()->CreateVertexShader(
			fileData.data(),
			fileData.size(),
			nullptr,
			&vertexShader
			)
		);

		App::ThrowIfFailed(
			deviceResources->GetD3DDevice()->CreateInputLayout(
			VertexTypes::VertexSkinned::ElementDesc,
			VertexTypes::VertexSkinned::ElementCount,
			fileData.data(),
			fileData.size(),
			&inputLayout
			)
		);
	});

	//After the pixel shader file is loaded, create the shader and constant buffer.
	auto createPSTask = loadPSTask.then([this](const std::vector<byte>& fileData)
	{
		App::ThrowIfFailed(
			deviceResources->GetD3DDevice()->CreatePixelShader(
			&fileData[0],
			fileData.size(),
			nullptr,
			&pixelShader
			)
			);
	});

	//Load shaders asynchronously.
	loadVSTask = App::ReadDataAsync(L"PositionColor.vs.cso");
	loadPSTask = App::ReadDataAsync(L"PositionColor.ps.cso");

	//After the vertex shader file is loaded, create the shader and input layout.
	createVSTask = loadVSTask.then([this](const std::vector<byte>& fileData)
	{
		App::ThrowIfFailed(
			deviceResources->GetD3DDevice()->CreateVertexShader(
			fileData.data(),
			fileData.size(),
			nullptr,
			&pcVertexShader
			)
		);

		App::ThrowIfFailed(
			deviceResources->GetD3DDevice()->CreateInputLayout(
			VertexTypes::VertexPositionColor::ElementDesc,
			VertexTypes::VertexPositionColor::ElementCount,
			fileData.data(),
			fileData.size(),
			&pcInputLayout
			)
		);
	});

	//After the pixel shader file is loaded, create the shader and constant buffer.
	createPSTask = loadPSTask.then([this](const std::vector<byte>& fileData)
	{
		App::ThrowIfFailed(
			deviceResources->GetD3DDevice()->CreatePixelShader(
			&fileData[0],
			fileData.size(),
			nullptr,
			&pcPixelShader
			)
		);
	});

	//Once both shaders are loaded, create the mesh.
	auto loadModelTask = Concurrency::create_task([this]()
	{
		Iqm::Load(deviceResources, ccache, "Content/test.iqm", iqm);
		iqmSkel = Mesh::Create(deviceResources, iqm.CreateSkeletalMesh());
	}).then([this]()
	{
		loadingComplete = true;
	});
}

void TestScene::ReleaseDeviceDependentResources()
{
	loadingComplete = false;
	
	vertexShader.Reset();
	inputLayout.Reset();
	pixelShader.Reset();

	pcVertexShader.Reset();
	pcInputLayout.Reset();
	pcPixelShader.Reset();
}

//Called once per frame, rotates the cube and calculates the model and view matrices.
void TestScene::Update(const DX::StepTimer& Timer)
{
	if (!tracking)
	{
		//Convert degrees to radians, then convert seconds to rotation angle
		float radiansPerSecond = XMConvertToRadians(degreesPerSecond);
		double totalRotation = Timer.GetTotalSeconds() * radiansPerSecond;
		float radians = static_cast<float>(fmod(totalRotation, XM_2PI));

		Rotate(radians);
	}
}

//Rotate the 3D cube model a set amount of radians.
void TestScene::Rotate(float Radians)
{
	//Prepare to pass the updated model matrix to the shader
	objectCBuffer.data.world = Matrix::CreateRotationY(Radians);
	objectCBuffer.data.world *= Matrix::CreateRotationX(XM_PIDIV2);
	objectCBuffer.data.Calc(cam.View(), cam.Projection());
}

void TestScene::StartTracking()
{
	tracking = true;
}

//When tracking, the 3D cube can be rotated around its Y axis by tracking pointer position relative to the output screen width.
void TestScene::TrackingUpdate(float PositionX)
{
	if (tracking)
	{
		float radians = XM_2PI * 2.0f * PositionX / deviceResources->GetOutputSize().Width;
		Rotate(radians);
	}
}

void TestScene::StopTracking()
{
	tracking = false;
}

//Renders one frame using the vertex and pixel shaders.
void TestScene::Render()
{
	if (!loadingComplete)
		return;

	auto context = deviceResources->GetD3DDeviceContext();

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	//context->RSSetState(wireRasterizer.Get());
	context->RSSetState(nullptr);
	context->IASetInputLayout(inputLayout.Get());

	context->VSSetShader(vertexShader.Get(), nullptr, 0);

	iqm.Skin(objectCBuffer.data);
	objectCBuffer.Update();
	objectCBuffer.BindVertex(0);

	context->PSSetShader(pixelShader.Get(), nullptr, 0);
	context->PSSetSamplers(0, 1, sampler.GetAddressOf());

	lightingCBuffer.BindPixel(1);
	materialCBuffer.BindPixel(0);

	iqm.Bind();
	for (auto& mesh : iqm.meshes)
	{
		materialCBuffer.data.FillFromMaterial(mesh.material);
		materialCBuffer.Update();

		if (mesh.material.texture != nullptr)
			mesh.material.texture->Apply();
		context->DrawIndexed((unsigned)mesh.IndexCount(), (unsigned)mesh.StartIndex(), 0);
	}

	context->ClearDepthStencilView(deviceResources->GetDepthStencilView(), D3D11_CLEAR_DEPTH, 1, 0);
	
	//Draw skeleton
	context->RSSetState(wireRasterizer.Get());

	context->IASetInputLayout(pcInputLayout.Get());

	pcCbuffer.data.wvp = objectCBuffer.data.worldViewProjection;
	pcCbuffer.Update();
	pcCbuffer.BindVertex(0);

	context->VSSetShader(pcVertexShader.Get(), nullptr, 0);
	context->PSSetShader(pcPixelShader.Get(), nullptr, 0);

	iqmSkel.Draw();
}