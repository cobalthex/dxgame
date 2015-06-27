#include "Pch.hpp"

#include "FpsDisplay.hpp"
#include "Engine/Common/PlatformHelpers.hpp"
#include "App/SystemSettings.hpp"
#include "Formats/DWriteFontCollectionLoader.hpp"
#include "Engine/Common/StringOps.hpp"

//Initializes D2D resources used for text rendering
FpsDisplay::FpsDisplay(const std::shared_ptr<DeviceResources>& DeviceResources)
	: lastFps(0), Drawable(DeviceResources)
{
	prefix = L"FPS: ";
	ZeroMemory(&textMetrics, sizeof(DWRITE_TEXT_METRICS));

	ComPtr<IDWriteFactory> dwFactory = deviceResources->GetDWriteFactory();

	auto fontFn = SystemSettings::GetFontFile(L"DebugFixed\\DebugFixed.ttf");
	ComPtr<IDWriteFontCollectionLoader> pCollectionLoader = new DWriteFontCollectionLoader(fontFn.data());
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

	screenTranslation = D2D1::Matrix3x2F::Translation(10, 10);

	CreateDeviceResources();
}
FpsDisplay::~FpsDisplay()
{
}

void FpsDisplay::CreateDeviceResources()
{
	Sys::ThrowIfFailed(deviceResources->GetD2DDeviceContext()->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &whiteBrush));
	Sys::ThrowIfFailed(deviceResources->GetD2DDeviceContext()->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black, 0.5f), &blackBrush));
}

void FpsDisplay::ReleaseDeviceResources()
{
	whiteBrush.Reset();
	blackBrush.Reset();
}

//Renders a frame to the screen
void FpsDisplay::Draw(const StepTimer& Timer)
{
	//Update display text
	unsigned fps = Timer.GetFramesPerSecond();

	//recreate fps text
	if (fps != lastFps || textLayout == nullptr)
	{
		auto text = prefix + std::to_wstring(fps) + suffix;
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
		lastFps = fps;
	}


	ID2D1DeviceContext* context = deviceResources->GetD2DDeviceContext();
	Windows::Foundation::Size logicalSize = deviceResources->GetLogicalSize();
	context->SaveDrawingState(stateBlock.Get());
	context->BeginDraw();

	context->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_ALIASED);

	context->SetTransform(screenTranslation * deviceResources->GetOrientationTransform2D());

	Sys::ThrowIfFailed(textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING));

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