#pragma once

#include "Pch.hpp"
#include "Common/DeviceResources.hpp"
#include "Common/StepTimer.hpp"

namespace DirectXGame
{
	// Renders the current FPS value in the bottom right corner of the screen using Direct2D and DirectWrite.
	class SampleFpsTextRenderer
	{
	public:
		SampleFpsTextRenderer(const std::shared_ptr<DX::DeviceResources>& DeviceResources);
		void CreateDeviceDependentResources();
		void ReleaseDeviceDependentResources();
		void Update(const DX::StepTimer& Timer);
		void Render();

	private:
		// Cached pointer to device resources.
		std::shared_ptr<DX::DeviceResources> deviceResources;

		// Resources related to text rendering.
		std::wstring                                    text;
		DWRITE_TEXT_METRICS	                            textMetrics;
		ComPtr<ID2D1SolidColorBrush>    whiteBrush;
		ComPtr<ID2D1SolidColorBrush>    blackBrush;

		ComPtr<ID2D1DrawingStateBlock>  stateBlock;
		ComPtr<IDWriteTextLayout>       textLayout;
		ComPtr<IDWriteTextFormat>		textFormat;
		size_t lastFps; //the last fps (for caching)
	};
}