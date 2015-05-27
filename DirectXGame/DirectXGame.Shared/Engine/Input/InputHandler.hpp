#pragma once

#include "Pch.hpp"
#include "Engine/Common/Math.hpp"

namespace InputHandler
{
	extern Math::Vector2 cursor;
	extern Math::Vector2 lastCursor;
 
	extern bool isLeftMousePressed;
	extern bool isMiddleMousePressed;
	extern bool isRightMousePressed;

	extern void Initialize(Windows::UI::Core::CoreWindow^ Window);
};