#include "Pch.hpp"
#include "FpsRenderer.hpp"

#include "Common/PlatformHelpers.hpp"

using namespace DirectXGame;

// Initializes D2D resources used for text rendering.
FpsRenderer::FpsRenderer(const std::shared_ptr<DX::DeviceResources>& DeviceResources)
	: text(L""), lastFps(0), Renderer(DeviceResources)
{
	ZeroMemory(&textMetrics, sizeof(DWRITE_TEXT_METRICS));

	// Create device independent resources
	DX::ThrowIfFailed(
		deviceResources->GetDWriteFactory()->CreateTextFormat(
			L"Segoe UI Mono",
			nullptr,
			DWRITE_FONT_WEIGHT_LIGHT,
			DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_NORMAL,
			16.0f,
			L"en-US",
			&textFormat
			)
		);

	DX::ThrowIfFailed(textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR));

	DX::ThrowIfFailed(deviceResources->GetD2DFactory()->CreateDrawingStateBlock(&stateBlock));

	CreateDeviceDependentResources();
}

// Updates the text to be displayed.
void FpsRenderer::Update(const DX::StepTimer& Timer)
{
	// Update display text.
	uint32_t fps = Timer.GetFramesPerSecond();

	text = (fps > 0) ? L"FPS: " + std::to_wstring(fps) : L"FPS: -";

	//cache fps text layout
	if (fps != lastFps || textLayout == nullptr)
	{
		DX::ThrowIfFailed
		(
			deviceResources->GetDWriteFactory()->CreateTextLayout
			(
				text.c_str(),
				(uint32_t)text.length(),
				textFormat.Get(),
				96.0f, // Max width of the input text.
				24.0f, // Max height of the input text.
				&textLayout
			)
		);

		DX::ThrowIfFailed(textLayout->GetMetrics(&textMetrics));
	}
	lastFps = fps;
}

// Renders a frame to the screen.
void FpsRenderer::Render()
{
	ID2D1DeviceContext* context = deviceResources->GetD2DDeviceContext();
	Windows::Foundation::Size logicalSize = deviceResources->GetLogicalSize();

	context->SaveDrawingState(stateBlock.Get());
	context->BeginDraw();

	// Position on the bottom right corner
	D2D1::Matrix3x2F screenTranslation = D2D1::Matrix3x2F::Translation(
		logicalSize.Width - textMetrics.layoutWidth - 8,
		logicalSize.Height - textMetrics.height - 2
		);

	context->SetTransform(screenTranslation * deviceResources->GetOrientationTransform2D());

	DX::ThrowIfFailed(
		textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING)
		);

	context->DrawTextLayout(D2D1::Point2F(-1.f, -1.f), textLayout.Get(), blackBrush.Get());
	context->DrawTextLayout(D2D1::Point2F(1.f, -1.f), textLayout.Get(), blackBrush.Get());
	context->DrawTextLayout(D2D1::Point2F(-1.f, 1.f), textLayout.Get(), blackBrush.Get());
	context->DrawTextLayout(D2D1::Point2F(1.f, 1.f), textLayout.Get(), blackBrush.Get());

	context->DrawTextLayout(D2D1::Point2F(0.f, 0.f), textLayout.Get(), whiteBrush.Get());

	// Ignore D2DERR_RECREATE_TARGET here. This error indicates that the device
	// is lost. It will be handled during the next call to Present.
	HRESULT hr = context->EndDraw();
	if (hr != D2DERR_RECREATE_TARGET)
		DX::ThrowIfFailed(hr);

	context->RestoreDrawingState(stateBlock.Get());
}

void FpsRenderer::CreateDeviceDependentResources()
{
	DX::ThrowIfFailed(deviceResources->GetD2DDeviceContext()->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &whiteBrush));
	DX::ThrowIfFailed(deviceResources->GetD2DDeviceContext()->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &blackBrush));
}
void FpsRenderer::ReleaseDeviceDependentResources()
{
	whiteBrush.Reset();
}