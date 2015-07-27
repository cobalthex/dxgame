#include "Pch.hpp"
#include "TestScene.hpp"
#include "Engine/Common/PlatformHelpers.hpp"
#include "Engine/Common/StringOps.hpp"
#include "Engine/Graphics/Models/Formats/IQM/IqmLoader.hpp"
#include "Engine/Graphics/Models/Formats/OBJ/ObjLoader.hpp"
#include "Engine/Graphics/Primitives.hpp"
#include "Engine/Data/Formats/Osl/Osl.hpp"
#include "App/AppData.hpp"

using namespace DirectX;
using namespace Windows::Foundation;
using namespace Windows::UI::Core;

//Loads vertex and pixel shaders from files and instantiates the cube geometry.
TestScene::TestScene(Game& Game, const DeviceResourcesPtr& DeviceResources)
	: loadingComplete(false),
	GameComponent(Game, DeviceResources)
{
	CreateDeviceResources();
	CreateWindowResources(DeviceResources->GetWindow().Get());
	loadingComplete = true;
}

//Initializes view parameters when the window size changes.
void TestScene::CreateWindowResources(CoreWindow^ Window)
{
	wnd = Platform::Agile<CoreWindow>(Window);

	Size outputSize = deviceResources->GetOutputSize();
	float aspectRatio = outputSize.Width / outputSize.Height;
	float fovAngleY = 70.0f * XM_PI / 180.0f;

	//This is a simple example of change that can be made when the app is in
	//portrait or snapped view.
	if (aspectRatio < 1.0f)
		fovAngleY *= 2.0f;

	cam.orientation = deviceResources->GetOrientationTransform3D();

	cam.fieldOfView = fovAngleY;
	cam.aspectRatio = aspectRatio;
	cam.nearPlaneDistance = 0.01f;
	cam.farPlaneDistance = 1000;

	cam.position = { 0, 50, 70 };
	cam.lookAt = { 0, 3, 0 };

	cam.CalcMatrices();
}

void TestScene::CreateDeviceResources()
{
	orc = game.LoadModel("mrfixit.iqm");
	cube = game.LoadModel("cube.iqm");
	map.world = game.LoadModel("tracktest.iqm");
	map.models.push_back(orc);
	map.models.push_back(cube);

	sp_cast<SkinnedModel>(orc)->AddCurrentPoseToTimeline(timeline);
	timeline.isLooping = true;
	timeline.Start();

	CreateStage(100);
}
void TestScene::CreateStage(float Radius)
{
	std::vector<Model::VertexType> verts;
	Model::VertexType v;
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

	std::map<std::string, ModelMesh> meshes;

	Material mtl;
	mtl.texture = game.LoadTexture("ground.dds");
	mtl.useTexture = (mtl.texture != nullptr && mtl.texture->IsValid());
	mtl.ambient = mtl.diffuse = Color(1, 1, 1);

	meshes["stage"] = { "stage", 0, 4, 0, 4, mtl, Bounds() };

	stage = Model(deviceResources, verts, indices, PrimitiveTopology::TriangleStrip, meshes);

	/*unsigned s = 128;
	unsigned* cmap = new unsigned[s * s];
	FillMemory(cmap, s * s * sizeof(unsigned), 0xff00ff00);
	mat.diffuseMap->SetData(cmap, 10, 10, s, s);*/
}


void TestScene::ReleaseDeviceResources()
{
	loadingComplete = false;
}

//Called once per frame, rotates the cube and calculates the model and view matrices.
void TestScene::Update(const StepTimer& Timer)
{
	if (!loadingComplete)
		return;

	Vector2 mouse = Vector2(wnd->PointerPosition.X, wnd->PointerPosition.Y);

	cam.CalcMatrices();

	timeline.Update();
	lastMouse = mouse;
}

void TestScene::Draw(const StepTimer& Timer)
{
	map.Draw(deviceResources->GetD3DDeviceContext(), cam, game.ShaderCache());
}