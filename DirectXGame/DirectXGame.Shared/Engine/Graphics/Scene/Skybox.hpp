#pragma once

#include "Engine/Graphics/DeviceResources.hpp"
#include "Engine/Graphics/Textures/Texture2D.hpp"
#include "Engine/Graphics/Models/Mesh.hpp"
#include "Engine/Common/Math.hpp"

class Skybox
{
public:
	Skybox(const DeviceResourcesPtr& DeviceResources, float Size = 10000);

protected:
	Mesh box;
	std::shared_ptr<Texture2D> cubeMap;
	//std::shared_ptr<PositionTextureShader> shader;

	Math::Matrix world;
};