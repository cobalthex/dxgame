#include "Pch.hpp"

#include "FpsDisplay.hpp"
#include "Engine/Common/PlatformHelpers.hpp"
#include "App/SystemSettings.hpp"

//Initializes D2D resources used for text rendering
FpsDisplay::FpsDisplay(const std::shared_ptr<DeviceResources>& DeviceResources, const std::shared_ptr<Shaders::TextShader>& Shader)
	: lastFps(0), textBuffer(nullptr), Drawable(DeviceResources), textColor(Color(1, 1, 1, 1.f))
{
	fontData = Sys::ReadFile(SystemSettings::GetFontFile("OCRAExt.ttf"));
	
	if (stbtt_InitFont(&fontInfo, fontData.data(), 0)) {}

	shader = Shader;
	CreateDeviceResources();
}
FpsDisplay::~FpsDisplay()
{
	if (textBuffer != nullptr)
		delete[] textBuffer;
}

void FpsDisplay::CreateDeviceResources()
{
	auto sz = deviceResources->GetLogicalSize();

	worldViewProjection
		= Matrix::CreateTranslation(0.01f, 0.01f, 0)
		* Matrix::CreateLookAt(Vector3::Zero, Vector3::Forward, Vector3::Up)
		//* Matrix::CreateOrthographicOffCenter(0, sz.Width, sz.Height, 0, -1, 1);
		* Matrix::CreateOrthographic(1000, 700, -100, 100);
}

void FpsDisplay::ReleaseDeviceResources()
{
}

//Renders a frame to the screen
void FpsDisplay::Draw(const StepTimer& Timer)
{
	//Update display text
	unsigned fps = Timer.GetFramesPerSecond();

	//recreate fps text
	if (fps != lastFps)
	{
		auto text = prefix + std::to_string(fps) + suffix;
		bitmap = BitmapTextMesh(deviceResources, text, &fontInfo, 24);
	}
	lastFps = fps;

	shader->color.data.color = textColor;
	shader->wvp.data.wvp = worldViewProjection;
	shader->Update();
	shader->Apply();
	shader->SetInputLayout();

	bitmap.Draw();
}