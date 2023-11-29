#pragma once
#include "KeyCodes.h"
#include "MouseButtonCodes.h"
#include "Math/Vector2.h"

namespace LevEngine
{
class Input
{
public:
	friend class Application;
	friend class Window;

	static bool IsKeyPressed(KeyCode keycode);
	static bool IsKeyReleased(KeyCode keycode);
	static bool IsKeyDown(KeyCode keycode);
	static bool IsKeyUp(KeyCode keycode);
	static bool IsMouseButtonPressed(MouseButton button);
	static bool IsMouseButtonReleased(MouseButton button);
	static bool IsMouseButtonDown(MouseButton button);
	static bool IsMouseButtonUp(MouseButton button);

	static float GetMouseWheelOffset();
	static Vector2 GetMousePosition();
	static Vector2 GetMouseDelta();
	static float GetMouseX();
	static float GetMouseY();

private:
	enum class State
	{
		Up,
		Down,
	};

	static UnorderedMap<KeyCode, State> s_CurrentKeyStates;
	static UnorderedMap<KeyCode, State> s_PreviousKeyStates;
	static UnorderedMap<MouseButton, State> s_ButtonStates;
	static UnorderedMap<MouseButton, State> s_PreviousButtonStates;

	static float s_MousePositionX;
	static float s_MousePositionY;
	static float s_MousePreviousX;
	static float s_MousePreviousY;

	static float s_MouseWheelOffset;

	static void OnKeyPressed(KeyCode keyCode);
	static void OnKeyReleased(KeyCode keyCode);
	static void OnMouseMoved(float x, float y);
	static void OnMouseButtonPressed(MouseButton mouseButton);
	static void OnMouseButtonReleased(MouseButton mouseButton);
	static void OnMouseScrolled(float offset);
	static void Reset();
};
}
