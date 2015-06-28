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
	texCache(DeviceResources),
	GameComponent(Game, DeviceResources)
{
	CreateDeviceResources();
	CreateWindowResources(DeviceResources->GetWindow().Get());
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

	cam.position = { 0, 10, 10 };
	cam.lookAt = { 0, 3, 0 };

	cam.CalcMatrices();
}

void TestScene::CreateDeviceResources()
{
	Obj::Load(deviceResources, AppData::GetModelFile("tracktest.obj"), texCache, game.LoadShader<Shaders::LitShader>(ShaderType::Lit), map.world);
}

void TestScene::ReleaseDeviceResources()
{
	loadingComplete = false;
}

//Called once per frame, rotates the cube and calculates the model and view matrices.
void TestScene::Update(const StepTimer& Timer)
{
	Vector2 mouse = Vector2(wnd->PointerPosition.X, wnd->PointerPosition.Y);



	timeline.Update();
	lastMouse = mouse;
}

//Renders one frame using the vertex and pixel shaders.
void TestScene::Draw(const StepTimer& Timer)
{
	if (!loadingComplete)
		return; 

	auto context = deviceResources->GetD3DDeviceContext();

	

	map.world.Draw();
}