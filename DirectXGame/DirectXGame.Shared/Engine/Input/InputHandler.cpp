#include "Pch.hpp"
#include "InputHandler.hpp"

using namespace Windows::Foundation;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;

Math::Vector2 InputHandler::cursor;
Math::Vector2 InputHandler::lastCursor;

bool InputHandler::isLeftMousePressed;
bool InputHandler::isMiddleMousePressed;
bool InputHandler::isRightMousePressed;

ref class _WinrtStatic
{
public:
	void PointerPressed(Windows::UI::Core::CoreWindow^ Sender, Windows::UI::Core::PointerEventArgs^ Args)
	{
		InputHandler::isLeftMousePressed = Args->CurrentPoint->Properties->IsLeftButtonPressed;
		InputHandler::isMiddleMousePressed = Args->CurrentPoint->Properties->IsMiddleButtonPressed;
		InputHandler::isRightMousePressed = Args->CurrentPoint->Properties->IsRightButtonPressed;
	}
	void PointerMoved(Windows::UI::Core::CoreWindow^ Sender, Windows::UI::Core::PointerEventArgs^ Args)
	{
		auto& cp = Args->CurrentPoint->Position;
		InputHandler::lastCursor = InputHandler::cursor;
		InputHandler::cursor.x = cp.X;
		InputHandler::cursor.y = cp.Y;
	}
	void PointerReleased(Windows::UI::Core::CoreWindow^ Sender, Windows::UI::Core::PointerEventArgs^ Args)
	{
		InputHandler::isLeftMousePressed = Args->CurrentPoint->Properties->IsLeftButtonPressed;
		InputHandler::isMiddleMousePressed = Args->CurrentPoint->Properties->IsMiddleButtonPressed;
		InputHandler::isRightMousePressed = Args->CurrentPoint->Properties->IsRightButtonPressed;
	}

} ^WinrtStatic;


void InputHandler::Initialize(Windows::UI::Core::CoreWindow^ Window)
{
	if (WinrtStatic == nullptr)
		WinrtStatic = ref new _WinrtStatic;

	Window->PointerPressed += ref new TypedEventHandler<CoreWindow^, PointerEventArgs^>(WinrtStatic, &_WinrtStatic::PointerPressed);
	Window->PointerMoved += ref new TypedEventHandler<CoreWindow^, PointerEventArgs^>(WinrtStatic, &_WinrtStatic::PointerMoved);
	Window->PointerReleased += ref new TypedEventHandler<CoreWindow^, PointerEventArgs^>(WinrtStatic, &_WinrtStatic::PointerReleased);
}