#pragma once

#include "Graphics/DeviceResources.hpp"
#include "Graphics/Textures/Texture3D.hpp"
#include "Graphics/Models/Mesh.hpp"
#include "Common/Math.hpp"

class Skybox
{
public:


protected:
	Mesh box;
	std::shared_ptr<Texture3D> cubeMap;
	//std::shared_ptr<PositionTextureShader> shader;

	DirectX::Math::Matrix world;
};