#pragma once

#include "Game/GameComponent.hpp"
#include "Engine/Graphics/Models/Model.hpp"
#include "Engine/Graphics/Models/SkinnedModel.hpp"
#include "Engine/Graphics/Textures/TextureCache.hpp"
#include "Engine/Graphics/Models/ModelCache.hpp"
#include "Engine/Graphics/Scene/Camera.hpp"
#include "Engine/Common/AlignedStorage.hpp"
#include "Engine/Animation/Timeline.hpp"
#include "Engine/Game/Maps/Map.hpp"

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
	bool loadingComplete;
	Math::Vector2 lastMouse;
	Platform::Agile<Windows::UI::Core::CoreWindow> wnd;

	Timeline timeline;

	Camera cam;

	Map map;
	std::shared_ptr<Model> cube;
	std::shared_ptr<Model> orc;

	void CreateStage(float Radius);

	Model stage;
};