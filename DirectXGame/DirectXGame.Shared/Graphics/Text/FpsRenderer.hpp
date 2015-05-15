#pragma once

#include "Pch.hpp"
#include "Game/GameComponent.hpp"
#include "Graphics/Textures/Texture2D.hpp"
#include "Common/Math.hpp"
#include "Graphics/Shaders/TextShader.hpp"
#include "Formats/StbFreetype.hpp"

//Renders the current FPS value in the bottom right corner of the screen using Direct2D and DirectWrite
class FpsRenderer : public GameComponent
{
public:
	FpsRenderer(Game& Game, const std::shared_ptr<DeviceResources>& DeviceResources);
	~FpsRenderer();
	void CreateDeviceResources();
	void ReleaseDeviceResources();
	void Update(const StepTimer& Timer);
	void Render();

	std::string prefix; //prefix before fps
	std::string suffix; //suffix after fps

	unsigned LastFps() const { return lastFps; } //the last recorded FPS

	Color textColor;

protected:
	unsigned lastFps;

	std::string fontBuffer;
	Texture2D tex;
	unsigned width, height;
	ComPtr<ID3D11Buffer> vertices;

	stbtt_fontinfo fontInfo;
	byte* buffer; //render buffer for Stb
	float scale;
	int ascent, descent, lineGap;

	std::shared_ptr<Shaders::TextShader> shader;
	DirectX::Math::Matrix worldViewProjection;
};