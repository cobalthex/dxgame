#pragma once

#include "Engine/Common/StepTimer.hpp"
#include "Engine/Graphics/DeviceResources.hpp"
#include "Engine/Graphics/Shaders/ShaderCache.hpp"
#include "Engine/Graphics/Text/FpsDisplay.hpp"

class GameComponent;

class Game : public IDeviceNotify
{
public:
	Game(const DeviceResourcesPtr& DeviceResources);
	~Game();
	void CreateWindowResources(Windows::UI::Core::CoreWindow^ Window); //Create window resources, note: will recalculate safe area
	void Update();
	bool Draw();

	//IDeviceNotify
	virtual void OnDeviceLost();
	virtual void OnDeviceRestored();

	inline std::shared_ptr<Shader> LoadShader(const ShaderType& Type) { return shaderCache.Load(Type); } //Load a shader from the game wide cache
	template <typename ShaderTy>
	inline std::shared_ptr<ShaderTy> LoadShader(const ShaderType& Type) { return shaderCache.Load<ShaderTy>(Type); } //Load a shade from the game wide cache
	inline ShaderCache& ShaderCache() { return shaderCache; }

	template <class T, typename Arg, typename... Args>
	inline void AddComponent(Arg&& Arg, Args&&... Args) { PushComponent(std::make_shared<T>(*this, deviceResources, Arg, Args...)); }
	template <class T>
	inline void AddComponent() { PushComponent(std::make_shared<T>(*this, deviceResources)); }

	void PushComponent(const std::shared_ptr<GameComponent>& GComponent); //Push a component onto the stack
	std::shared_ptr<GameComponent> PopComponent(); //remove the top most component and deactivate it (Returns nullptr if no components)

	Windows::Foundation::Rect safeArea; //the safe area of the game (all important content including UI should stay within this region) - Usually should be around 90% of the screen

private:
	std::vector<std::shared_ptr<GameComponent>> components; //all of the game compoinents managed by this game

	//Cached pointer to device resources.
	DeviceResourcesPtr deviceResources;

	//Rendering loop timer
	StepTimer timer;

	::ShaderCache shaderCache;
	FpsDisplay FpsDisplay;
};