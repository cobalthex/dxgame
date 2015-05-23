#pragma once

#include "Engine/Common/StepTimer.hpp"
#include "Engine/Graphics/DeviceResources.hpp"
#include "Engine/Graphics/Shaders/ShaderCache.hpp"
#include "Engine/Graphics/Text/FpsRenderer.hpp"

class GameComponent;

class Game : public IDeviceNotify
{
public:
	Game(const DeviceResourcesPtr& DeviceResources);
	~Game();
	void CreateWindowResources(Windows::UI::Core::CoreWindow^ Window);
	void Update();
	bool Draw();

	//IDeviceNotify
	virtual void OnDeviceLost();
	virtual void OnDeviceRestored();

	inline std::shared_ptr<Shader> LoadShader(const ShaderType& Type) { return shaderCache.Load(Type); } //Load a shader from the game wide cache
	template <typename ShaderTy>
	inline std::shared_ptr<ShaderTy> LoadShader(const ShaderType& Type) { return shaderCache.Load<ShaderTy>(Type); } //Load a shade from the game wide cache

private:
	//Cached pointer to device resources.
	DeviceResourcesPtr deviceResources;

	std::vector<std::shared_ptr<GameComponent>> components; //all of the game compoinents managed by this game

	//Rendering loop timer
	StepTimer timer;

	ShaderCache shaderCache;
	FpsRenderer fpsRenderer;
};