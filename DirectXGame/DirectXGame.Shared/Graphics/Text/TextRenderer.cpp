#include "Pch.hpp"
#include "TextRenderer.hpp"
#include <strsafe.h>
#include <Winbase.h>

using namespace DirectXGame;

//Initializes D2D resources used for text rendering
TextRenderer::TextRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources)
	: deviceResources(deviceResources)
{
	App::ThrowIfFailed(deviceResources->GetD2DFactory()->CreateDrawingStateBlock(&state));
}

//Renders a frame to the screen
void TextRenderer::Render(const std::wstring& Text, const TextLayoutParams& Params, const D2DBrush& Brush, const D2D1::Matrix3x2F& Position, const D2D_POINT_2F& Origin)
{
	//create layout
	DWTextLayout layout;
	App::ThrowIfFailed
	(
		deviceResources->GetDWriteFactory()->CreateTextLayout
		(
			Text.c_str(),
			(uint32)Text.length(),
			Params.textFormat.Get(),
			Params.maxWidth,
			Params.maxHeight,
			&layout
		)
	);

	Render(layout, Brush, Position, Origin);
}

void TextRenderer::Render(const DWTextLayout& Layout, const D2DBrush& Brush, const D2D1::Matrix3x2F& Position, const D2D_POINT_2F& Origin)
{
	ID2D1DeviceContext* context = deviceResources->GetD2DDeviceContext();
	Windows::Foundation::Size logicalSize = deviceResources->GetLogicalSize();

	context->SaveDrawingState(state.Get());
	context->BeginDraw();

	context->SetTransform(Position * deviceResources->GetOrientationTransform2D());
	context->DrawTextLayout(Origin, Layout.Get(), Brush.Get());

	//Ignore D2DERR_RECREATE_TARGET here This error indicates that the device
	//is lost. It will be handled during the next call to Present
	HRESULT hr = context->EndDraw();
	if (hr != D2DERR_RECREATE_TARGET)
		App::ThrowIfFailed(hr);

	context->RestoreDrawingState(state.Get());
}