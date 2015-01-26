#include "Pch.hpp"
#include "SampleFpsTextRenderer.hpp"

#include "Common/PlatformHelpers.hpp"

using namespace DirectXGame;

// Initializes D2D resources used for text rendering.
SampleFpsTextRenderer::SampleFpsTextRenderer(const std::shared_ptr<DX::DeviceResources>& DeviceResources) : 
	text(L""),
	deviceResources(DeviceResources)
{
	ZeroMemory(&textMetrics, sizeof(DWRITE_TEXT_METRICS));

	// Create device independent resources
	DX::ThrowIfFailed(
		deviceResources->GetDWriteFactory()->CreateTextFormat(
			L"Segoe UI",
			nullptr,
			DWRITE_FONT_WEIGHT_LIGHT,
			DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_NORMAL,
			32.0f,
			L"en-US",
			&textFormat
			)
		);

	DX::ThrowIfFailed(
		textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR)
		);

	DX::ThrowIfFailed(
		deviceResources->GetD2DFactory()->CreateDrawingStateBlock(&stateBlock)
		);

	CreateDeviceDependentResources();
}

// Updates the text to be displayed.
void SampleFpsTextRenderer::Update(const DX::StepTimer& Timer)
{
	// Update display text.
	uint32_t fps = Timer.GetFramesPerSecond();

	text = (fps > 0) ? std::to_wstring(fps) + L" FPS" : L" - FPS";

	DX::ThrowIfFailed(
		deviceResources->GetDWriteFactory()->CreateTextLayout(
			text.c_str(),
			(uint32_t) text.length(),
			textFormat.Get(),
			240.0f, // Max width of the input text.
			50.0f, // Max height of the input text.
			&textLayout
			)
		);

	DX::ThrowIfFailed(
		textLayout->GetMetrics(&textMetrics)
		);
}

// Renders a frame to the screen.
void SampleFpsTextRenderer::Render()
{
	ID2D1DeviceContext* context = deviceResources->GetD2DDeviceContext();
	Windows::Foundation::Size logicalSize = deviceResources->GetLogicalSize();

	context->SaveDrawingState(stateBlock.Get());
	context->BeginDraw();

	// Position on the bottom right corner
	D2D1::Matrix3x2F screenTranslation = D2D1::Matrix3x2F::Translation(
		logicalSize.Width - textMetrics.layoutWidth,
		logicalSize.Height - textMetrics.height
		);

	context->SetTransform(screenTranslation * deviceResources->GetOrientationTransform2D());

	DX::ThrowIfFailed(
		textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING)
		);

	context->DrawTextLayout(
		D2D1::Point2F(0.f, 0.f),
		textLayout.Get(),
		whiteBrush.Get()
		);

	// Ignore D2DERR_RECREATE_TARGET here. This error indicates that the device
	// is lost. It will be handled during the next call to Present.
	HRESULT hr = context->EndDraw();
	if (hr != D2DERR_RECREATE_TARGET)
	{
		DX::ThrowIfFailed(hr);
	}

	context->RestoreDrawingState(stateBlock.Get());
}

void SampleFpsTextRenderer::CreateDeviceDependentResources()
{
	DX::ThrowIfFailed(
		deviceResources->GetD2DDeviceContext()->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &whiteBrush)
		);
}
void SampleFpsTextRenderer::ReleaseDeviceDependentResources()
{
	whiteBrush.Reset();
}