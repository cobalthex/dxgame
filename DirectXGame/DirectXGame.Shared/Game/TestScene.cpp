#include "Pch.hpp"
#include "TestScene.hpp"
#include "Engine/Common/PlatformHelpers.hpp"
#include "Engine/Common/StringOps.hpp"
#include "Engine/Graphics/Models/Formats/IQM/IqmLoader.hpp"
#include "Engine/Graphics/Models/Formats/OBJ/ObjLoader.hpp"
#include "Engine/Graphics/Primitives.hpp"
#include "Input/InputHandler.hpp"
#include "Engine/Data/Formats/Osl/Osl.hpp"
#include "../Graphics/Shaders/Shader.hpp"

using namespace DirectXGame;

using namespace DirectX;
using namespace Windows::Foundation;
using namespace Windows::UI::Core;

//Loads vertex and pixel shaders from files and instantiates the cube geometry.
TestScene::TestScene(Game& Game, const std::shared_ptr<DeviceResources>& DeviceResources) :
	loadingComplete(false),
	degreesPerSecond(45),
	tracking(false),
	texCache(DeviceResources),
	GameComponent(Game, DeviceResources)
{
	CreateDeviceResources();
	CreateWindowResources(DeviceResources->GetWindow().Get());
	
}

//Initializes view parameters when the window size changes.
void TestScene::CreateWindowResources(CoreWindow^ Window)
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

void TestScene::CreateDeviceResources()
{
	lsShader = game.LoadShader<Shaders::LitSkinnedShader>(ShaderType::LitSkinned);
	ulShader = game.LoadShader<Shaders::UnlitShader>(ShaderType::Unlit);
	pcShader = game.LoadShader<Shaders::PositionColorShader>(ShaderType::PositionColor);

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

	CreateStage();

	//Once both shaders are loaded, create the mesh.
	auto loadModelTask = Concurrency::create_task([this]()
	{
		Iqm::Load(deviceResources, "Content/Models/mrfixit.iqm", texCache, lsShader, iqm);
		iqmSkel = Mesh::Create(deviceResources, iqm.CreateSkeletalMesh(Color(1, 0.6f, 0, 0)), true);

		timeline.Add(&iqm.poses[iqm.pose]);
		timeline.isLooping = true;
		timeline.Start();
		
		Obj::Load(deviceResources, "Content/Models/sword.obj", texCache, game.LoadShader<Shaders::LitShader>(ShaderType::Lit), sword);

	}).then([this]()
	{
		loadingComplete = true;
	});
}

void TestScene::CreateStage(float Radius)
{
	std::vector<StaticModel::VertexType> verts;
	StaticModel::VertexType v;
	v.position = Vector3(-Radius, 0, -Radius);
	v.texCoord = Vector2(0, 0);
	v.color = DirectX::PackedVector::XMUBYTEN4(0.5f, 0.5f, 0.5f, 1);
	v.normal = Vector3::Up;
	verts.push_back(v);
	
	v.position = Vector3(Radius, 0, -Radius);
	v.texCoord = Vector2(1, 0);
	v.normal = Vector3::Up;
	verts.push_back(v);

	v.position = Vector3(-Radius, 0, Radius);
	v.texCoord = Vector2(0, 1);
	v.normal = Vector3::Up;
	verts.push_back(v);

	v.position = Vector3(Radius, 0, Radius);
	v.texCoord = Vector2(1, 1);
	v.normal = Vector3::Up;
	verts.push_back(v);

	static const std::vector<unsigned> indices = { 0, 1, 2, 3 };

	std::map<std::string, StaticModel::MeshType> meshes;
	
	Materials::LitMaterial mat(game.LoadShader<Shaders::LitShader>(ShaderType::Lit));
	mat.diffuseMap = texCache.Load("Stage.dds");
	mat.useTexture = mat.diffuseMap->IsValid();
	mat.ambient = mat.diffuse = Color(1, 1, 1);

	meshes["stage"] = { "stage", 0, 4, 0, 4, mat, Bounds() };

	stage = StaticModel(deviceResources, verts, indices, PrimitiveTopology::TriangleStrip, meshes);

	camRotation = { 0, -0.4f, 12 };
}

void TestScene::ReleaseDeviceResources()
{
	loadingComplete = false;
}

//Called once per frame, rotates the cube and calculates the model and view matrices.
void TestScene::Update(const StepTimer& Timer)
{
	Matrix world =
		Matrix::CreateRotationX(-XM_PIDIV2) *
		Matrix::CreateRotationY(-XM_PIDIV2);

	cam.position = Vector3::Transform(Vector3::Backward, Matrix::CreateFromYawPitchRoll(camRotation.x, camRotation.y, 0));
	cam.position *= camRotation.z;
	cam.CalcMatrices();
	

	lsShader->object.data.world = world.Transpose();
	lsShader->object.data.Calc(cam.View(), cam.Projection());

	auto sh = (Shaders::LitShader*)(stage.meshes.begin()->second.material.shader.get());
	sh->lighting = lsShader->lighting;
	sh->object.data.world = Matrix::Identity;
	sh->object.data.Calc(cam.View(), cam.Projection());

	if (InputHandler::isLeftMousePressed)
	{
		auto xy = InputHandler::cursor - InputHandler::lastCursor;
		xy /= deviceResources->GetDpi();
		camRotation.x -= xy.x;
		camRotation.y -= xy.y;
		//camRotation.z -= 
	}

	timeline.Update();
}

//Renders one frame using the vertex and pixel shaders.
void TestScene::Render()
{
	if (!loadingComplete)
		return;

	

	auto context = deviceResources->GetD3DDeviceContext();
	context->RSSetState(nullptr);

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	
	auto sh = (Shaders::LitShader*)(stage.meshes.begin()->second.material.shader.get());
	sh->SetInputLayout();
	sh->Apply();
	sh->Update();
	stage.Draw();

	//sword.Draw();

	
	ulShader->Apply();
	ulShader->Update();
	ulShader->object = sh->object;
	sword.BeginDraw();
	for (auto& m : sword.meshes)
	{
		m.second.material.diffuseMap->Apply();
		m.second.Draw(context);
	}

	/*
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	lsShader->SetInputLayout();
	lsShader->Apply();

	iqm.Skin(lsShader->object.data.joints);
	lsShader->object.Update();

	iqm.BeginDraw();
	context->DrawAuto();
	iqm.Draw();
	
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
	*/
}