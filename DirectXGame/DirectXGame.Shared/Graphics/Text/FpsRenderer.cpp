#include "Pch.hpp"
#include "FpsRenderer.hpp"
#include "Common/PlatformHelpers.hpp"
#include "DWriteFontCollectionLoader.hpp"
#include "App/SystemSettings.hpp"

using namespace DirectXGame;

//Initializes D2D resources used for text rendering
FpsRenderer::FpsRenderer(const std::shared_ptr<DeviceResources>& DeviceResources)
	: text(L""), lastFps(0), Renderer(DeviceResources)
{
	ZeroMemory(&textMetrics, sizeof(DWRITE_TEXT_METRICS));

	ComPtr<IDWriteFactory> dwFactory = deviceResources->GetDWriteFactory();

	auto fontFilename = CombinePaths(Sys::GetWorkingDirectory(), ToWString(StringReplace(SystemSettings::GetFontFile("DebugFixed/DebugFixed.ttf"), "/", "\\")));

	ComPtr<IDWriteFontCollectionLoader> pCollectionLoader = new DWriteFontCollectionLoader(fontFilename.data());
	Sys::ThrowIfFailed(dwFactory->RegisterFontCollectionLoader(pCollectionLoader.Get()));

	ComPtr<IDWriteFontCollection> pFontCollection;
	Sys::ThrowIfFailed(dwFactory->CreateCustomFontCollection(pCollectionLoader.Get(), nullptr, 0, &pFontCollection));

	//Create device independent resources
	Sys::ThrowIfFailed(
		dwFactory->CreateTextFormat(
		L"DebugFixed",
		pFontCollection.Get(),
		DWRITE_FONT_WEIGHT_NORMAL,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		16.0f,
		L"en-US",
		&textFormat
		)
	);

	Sys::ThrowIfFailed(textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR));

	Sys::ThrowIfFailed(deviceResources->GetD2DFactory()->CreateDrawingStateBlock(&stateBlock));

	CreateDeviceResources();
}

//Updates the text to be displayed
void FpsRenderer::Update(const StepTimer& Timer)
{
	//Update display text
	uint32_t fps = Timer.GetFramesPerSecond();

	text = (fps > 0) ? L"FPS: " + std::to_wstring(fps) : L"FPS: -";

	//cache fps text layout
	if (fps != lastFps || textLayout == nullptr)
	{
		Sys::ThrowIfFailed
		(
			deviceResources->GetDWriteFactory()->CreateTextLayout
			(
				text.c_str(),
				(uint32_t)text.length(),
				textFormat.Get(),
				64.0f, //Max width of the input text
				16.0f, //Max height of the input text
				&textLayout
			)
		);

		Sys::ThrowIfFailed(textLayout->GetMetrics(&textMetrics));
	}
	lastFps = fps;
}

//Renders a frame to the screen
void FpsRenderer::Render()
{
	ID2D1DeviceContext* context = deviceResources->GetD2DDeviceContext();
	Windows::Foundation::Size logicalSize = deviceResources->GetLogicalSize();
	context->SaveDrawingState(stateBlock.Get());
	context->BeginDraw();

	context->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_ALIASED);

	//Position on the bottom right corner
	D2D1::Matrix3x2F screenTranslation = D2D1::Matrix3x2F::Translation(
		logicalSize.Width - textMetrics.layoutWidth - 8,
		logicalSize.Height - textMetrics.height - 2
		);

	context->SetTransform(screenTranslation * deviceResources->GetOrientationTransform2D());

	Sys::ThrowIfFailed(textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING));

	context->DrawTextLayout(D2D1::Point2F(-1.f, -1.f), textLayout.Get(), blackBrush.Get());
	context->DrawTextLayout(D2D1::Point2F(1.f, -1.f), textLayout.Get(), blackBrush.Get());
	context->DrawTextLayout(D2D1::Point2F(-1.f, 1.f), textLayout.Get(), blackBrush.Get());
	context->DrawTextLayout(D2D1::Point2F(1.f, 1.f), textLayout.Get(), blackBrush.Get());

	context->DrawTextLayout(D2D1::Point2F(0.f, 0.f), textLayout.Get(), whiteBrush.Get());

	//Ignore D2DERR_RECREATE_TARGET here This error indicates that the device
	//is lost. It will be handled during the next call to Present
	HRESULT hr = context->EndDraw();
	if (hr != D2DERR_RECREATE_TARGET)
		Sys::ThrowIfFailed(hr);

	context->RestoreDrawingState(stateBlock.Get());
}

void FpsRenderer::CreateDeviceResources()
{
	Sys::ThrowIfFailed(deviceResources->GetD2DDeviceContext()->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &whiteBrush));
	Sys::ThrowIfFailed(deviceResources->GetD2DDeviceContext()->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black, 0.5f), &blackBrush));
}
void FpsRenderer::ReleaseDeviceResources()
{
	whiteBrush.Reset();
}