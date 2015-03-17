#pragma once

#include "Pch.hpp"
#include "Graphics/DeviceResources.hpp"
#include "Common/PlatformHelpers.hpp"

namespace DirectXGame
{
	typedef Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> D2DBrush;
	typedef Microsoft::WRL::ComPtr<IDWriteTextFormat> DWTextFormat;
	typedef Microsoft::WRL::ComPtr<IDWriteTextLayout> DWTextLayout;

	//Params for creating a new text layout
	struct TextLayoutParams
	{
		float			maxWidth;	//the max width of the text region (in pixels)
		float			maxHeight;	//the max height of the text region (in pixels)
		DWTextFormat	textFormat;	//The format (style: font, size, etc) of the text to draw

		//A direct way to create a text format Throws on error
		static inline DWTextFormat CreateTextFormat
		(
			const std::shared_ptr<DX::DeviceResources>& DeviceResources,
			const std::wstring& FontFamily,
			float Size,
			DWRITE_FONT_WEIGHT Weight = DWRITE_FONT_WEIGHT_NORMAL,
			DWRITE_FONT_STYLE Style = DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH Stretch = DWRITE_FONT_STRETCH_NORMAL,
			DWRITE_PARAGRAPH_ALIGNMENT DefaultAlignment = DWRITE_PARAGRAPH_ALIGNMENT_NEAR,
			const std::wstring& Locale = L"en-US",
			Microsoft::WRL::ComPtr<IDWriteFontCollection> FontCollection = nullptr
		)
		{
			DWTextFormat format;
			//Create device independent resources
			Sys::ThrowIfFailed
			(
				DeviceResources->GetDWriteFactory()->CreateTextFormat
				(
					FontFamily.c_str(),
					FontCollection.Get(),
					Weight,
					Style,
					Stretch,
					Size,
					Locale.c_str(),
					&format
				)
			);
			Sys::ThrowIfFailed(format->SetParagraphAlignment(DefaultAlignment));

			return format;
		}
	};

	//Renders text using Direct2D and DirectWrite
	class TextRenderer
	{
	public:
		TextRenderer(const std::shared_ptr<DX::DeviceResources>& DeviceResources);

		void Render(const std::wstring& Text, const TextLayoutParams& Params, const D2DBrush& Brush, const D2D1::Matrix3x2F& Position, const D2D_POINT_2F& Origin = D2D1::Point2F());
		void Render(const DWTextLayout& Layout, const D2DBrush& Brush, const D2D1::Matrix3x2F& Position, const D2D_POINT_2F& Origin = D2D1::Point2F());
		
	private:
		//Cached pointer to device resources
		std::shared_ptr<DX::DeviceResources> deviceResources;
		Microsoft::WRL::ComPtr<ID2D1DrawingStateBlock> state;
	};
}