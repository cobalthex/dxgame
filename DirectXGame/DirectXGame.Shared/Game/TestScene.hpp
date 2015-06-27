#pragma once

#include "Game/GameComponent.hpp"
#include "Engine/Graphics/Models/SkinnedModel.hpp"
#include "Engine/Graphics/Textures/TextureCache.hpp"
#include "Engine/Graphics/Textures/Texture2D.hpp"
#include "Engine/Graphics/Shaders/UnlitShader.hpp"
#include "Engine/Graphics/ConstantBuffer.hpp"
#include "Engine/Graphics/Lighting.hpp"
#include "Engine/Graphics/Scene/Camera.hpp"
#include "Engine/Common/AlignedStorage.hpp"
#include "Engine/Animation/Timeline.hpp"
#include "Engine/Graphics/Shaders/ShaderCache.hpp"

// This sample renderer instantiates a basic rendering pipeline.
class TestScene : public AlignedStorage<16>, public GameComponent
{
public:
	TestScene(Game& Game, const DeviceResourcesPtr& DeviceResources);
	void CreateDeviceResources() override;
	void CreateWindowResources(Windows::UI::Core::CoreWindow^ Window) override;
	void ReleaseDeviceResources() override;
	void Update(const StepTimer& Timer);
	void Draw(const StepTimer& Timer);

private:
	TextureCache texCache;

	std::shared_ptr<Shaders::LitSkinnedShader> lsShader;
	std::shared_ptr<Shaders::UnlitShader> ulShader;
	std::shared_ptr<Shaders::PositionColorShader> pcShader;

	ComPtr<ID3D11SamplerState>	sampler;
	ComPtr<ID3D11RasterizerState> wireRasterizer;

	Camera cam;
	Vector3 camRotation;

	SkinnedModel iqm;
	Mesh iqmSkel;
	Texture2D* tex;

	Model stage;
	void CreateStage(float Radius = 10);

	Model sword;
	
	Timeline timeline;

	// Variables used with the rendering loop.
	bool loadingComplete;

	Math::Vector2 lastMouse;

	Platform::Agile<Windows::UI::Core::CoreWindow> wnd;
};