#pragma once

#include "Pch.hpp"
#include "Engine/Graphics/Textures/Texture2D.hpp"
#include "Engine/Common/Math.hpp"
#include "Engine/Graphics/Drawable.hpp"

//Renders the current FPS value in the bottom right corner of the screen using Direct2D and DirectWrite
class FpsDisplay : public Drawable
{
public:
	FpsDisplay(const std::shared_ptr<DeviceResources>& DeviceResources);
	virtual ~FpsDisplay();

	void CreateDeviceResources();
	void ReleaseDeviceResources();
	void Draw(const StepTimer& Timer);

	std::wstring prefix; //prefix before fps
	std::wstring suffix; //suffix after fps

	unsigned LastFps() const { return lastFps; } //the last recorded FPS

protected:
	unsigned lastFps;

	Math::Matrix worldViewProjection;

	std::wstring                    text;
	DWRITE_TEXT_METRICS	            textMetrics;
	ComPtr<ID2D1SolidColorBrush>    whiteBrush;
	ComPtr<ID2D1SolidColorBrush>    blackBrush;

	ComPtr<ID2D1DrawingStateBlock>  stateBlock;
	ComPtr<IDWriteTextLayout>       textLayout;
	ComPtr<IDWriteTextFormat>		textFormat;

	D2D1::Matrix3x2F screenTranslation;
};