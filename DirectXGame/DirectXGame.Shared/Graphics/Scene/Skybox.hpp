#pragma once

#include "Graphics/DeviceResources.hpp"
#include "Graphics/Textures/Texture2D.hpp"
#include "Graphics/Models/Mesh.hpp"
#include "Common/Math.hpp"

class Skybox
{
public:
	Skybox(const DeviceResourcesPtr& DeviceResources, float Size = 10000);

protected:
	Mesh box;
	std::shared_ptr<Texture2D> cubeMap;
	//std::shared_ptr<PositionTextureShader> shader;

	DirectX::Math::Matrix world;
};