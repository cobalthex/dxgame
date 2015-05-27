#pragma once

#include "Pch.hpp"
#include "Engine/Graphics/Textures/Texture2D.hpp"
#include "Engine/Common/Math.hpp"
#include "Engine/Graphics/Shaders/TextShader.hpp"
#include "Formats/StbFreetype.hpp"
#include "Engine/Graphics/Drawable.hpp"

//Renders the current FPS value in the bottom right corner of the screen using Direct2D and DirectWrite
class FpsDisplay : public Drawable
{
public:
	FpsDisplay(const std::shared_ptr<DeviceResources>& DeviceResources, const std::shared_ptr<Shaders::TextShader>& Shader);
	virtual ~FpsDisplay();

	void CreateDeviceResources();
	void ReleaseDeviceResources();
	void Draw(const StepTimer& Timer);

	std::string prefix; //prefix before fps
	std::string suffix; //suffix after fps

	unsigned LastFps() const { return lastFps; } //the last recorded FPS

	Color textColor;

protected:
	unsigned lastFps;

	Texture2D tex;
	unsigned width, height;
	ComPtr<ID3D11Buffer> vertices;

	std::vector<byte> fontData;
	stbtt_fontinfo fontInfo;
	byte* textBuffer;
	float scale;
	int ascent, descent, lineGap, baseline;

	std::shared_ptr<Shaders::TextShader> shader;
	Math::Matrix worldViewProjection;
};