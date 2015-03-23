#include "Pch.hpp"
#include "TestScene.hpp"
#include "Common/PlatformHelpers.hpp"
#include "Common/Helpers.hpp"
#include "Graphics/Models/Formats/IQM/IqmLoader.hpp"

#include "Data/Formats/Osl/Osl.hpp"

using namespace DirectXGame;

using namespace DirectX;
using namespace Windows::Foundation;

//Loads vertex and pixel shaders from files and instantiates the cube geometry.
TestScene::TestScene(const std::shared_ptr<DeviceResources>& DeviceResources) :
	loadingComplete(false),
	degreesPerSecond(45),
	tracking(false),
	Renderer(DeviceResources),
	shCache(DeviceResources),
	texCache(DeviceResources)
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

	deviceResources->GetD3DDeviceContext()->PSSetSamplers(0, 1, sampler.GetAddressOf());

	cam.orientation = deviceResources->GetOrientationTransform3D();

	cam.fieldOfView = fovAngleY;
	cam.aspectRatio = aspectRatio;
	cam.nearPlaneDistance = 0.01f;
	cam.farPlaneDistance = 1000;

	cam.position = { 0, 10, 10 };
	cam.lookAt = { 0, 3, 0 };

	cam.CalcMatrices();

	D3D11_RASTERIZER_DESC rastDesc;
	ZeroMemory(&rastDesc, sizeof(D3D11_RASTERIZER_DESC));
	rastDesc.FillMode = D3D11_FILL_WIREFRAME;
	rastDesc.CullMode = D3D11_CULL_NONE;
	rastDesc.AntialiasedLineEnable = true;
	rastDesc.MultisampleEnable = true;
	Sys::ThrowIfFailed(deviceResources->GetD3DDevice()->CreateRasterizerState(&rastDesc, &wireRasterizer));
}

void TestScene::CreateDeviceDependentResources()
{
	lsShader = std::static_pointer_cast<Shaders::LitSkinnedShader>(shCache.Load(ShaderType::LitSkinned));
	pcShader = std::static_pointer_cast<Shaders::PositionColorShader>(shCache.Load(ShaderType::PositionColor));

	Light li;
	li.color = Color(1, 1, 1, 1);
	li.spotAngle = XMConvertToRadians(45);
	li.constantAttenuation = 1;
	li.linearAttenuation = 0.08f;
	li.quadraticAttenuation = 0;
	li.position = Vector4(5, 6, 3, 0);
	li.direction = -li.position;
	li.isEnabled = true;
	li.type = LightType::Point;

	lsShader->lighting.data.lights[0] = li;
	lsShader->lighting.data.eyePosition = Vector4(cam.position);
	lsShader->lighting.data.globalAmbience = Color(0, 0, 0, 0);
	lsShader->lighting.Update();

	//Once both shaders are loaded, create the mesh.
	auto loadModelTask = Concurrency::create_task([this]()
	{
		Iqm::Load(deviceResources, texCache, "Content/test.iqm", iqm);
		iqmSkel = Mesh::Create(deviceResources, iqm.CreateSkeletalMesh(Color(1, 0.6f, 0, 0)), true);

		timeline.Add(&iqm.poses[iqm.pose]);
		timeline.isLooping = true;
		timeline.Start();
	}).then([this]()
	{
		loadingComplete = true;
	});
}

void TestScene::ReleaseDeviceDependentResources()
{
	loadingComplete = false;
}

//Called once per frame, rotates the cube and calculates the model and view matrices.
void TestScene::Update(const StepTimer& Timer)
{
	if (!tracking)
	{
		//Convert degrees to radians, then convert seconds to rotation angle
		float radiansPerSecond = XMConvertToRadians(degreesPerSecond);
		double totalRotation = Timer.GetTotalSeconds() * radiansPerSecond;
		float radians = static_cast<float>(fmod(totalRotation, XM_2PI));

		Rotate(radians);
	}

	timeline.Update();
}

//Rotate the 3D cube model a set amount of radians.
void TestScene::Rotate(float Radians)
{
	Matrix world =
		Matrix::CreateRotationX(-XM_PIDIV2) *
		Matrix::CreateRotationY(-XM_PIDIV2);
		//Matrix::CreateRotationY(-Radians);

	lsShader->object.data.world = world.Transpose();
	lsShader->object.data.Calc(cam.View(), cam.Projection());
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
	
	context->RSSetState(nullptr);
	context->IASetInputLayout(inputLayout.Get());

	lsShader->SetInputLayout();
	lsShader->Apply();

	iqm.Skin(lsShader->object.data.joints);
	lsShader->object.Update();

	iqm.Bind();
	for (auto& mesh : iqm.meshes)
	{
		lsShader->material.data.FillFromMaterial(mesh.material);
		lsShader->material.Update();

		if (mesh.material.texture != nullptr)
			mesh.material.texture->Apply();
		context->DrawIndexed((unsigned)mesh.IndexCount(), (unsigned)mesh.StartIndex(), 0);
	}

	context->ClearDepthStencilView(deviceResources->GetDepthStencilView(), D3D11_CLEAR_DEPTH, 1, 0);

	auto skel = iqm.CreateSkeletalMesh(Color(1, 0.6f, 0, 0));
	iqmSkel.UpdateVertices(skel.vertices);
	
	//Draw skeleton
	context->RSSetState(wireRasterizer.Get());

	pcShader->wvp.data.wvp = lsShader->object.data.worldViewProjection;
	pcShader->SetInputLayout();
	pcShader->Update();
	pcShader->Apply();

	iqmSkel.Draw();
	context->RSSetState(nullptr);
}