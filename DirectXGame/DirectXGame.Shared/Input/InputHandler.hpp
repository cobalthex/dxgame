#pragma once

#include "Pch.hpp"
#include "Common/SimpleMath.hpp"

namespace InputHandler
{
	extern DirectX::SimpleMath::Vector2 cursor;
	extern DirectX::SimpleMath::Vector2 lastCursor;
 
	extern bool isLeftMousePressed;
	extern bool isMiddleMousePressed;
	extern bool isRightMousePressed;

	extern void Initialize(Windows::UI::Core::CoreWindow^ Window);
};