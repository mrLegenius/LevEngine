#pragma once
#include <unordered_map>
#include "KeyCodes.h"
#include "MouseButtonCodes.h"

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
	static std::pair<float, float> GetMousePosition();
	static std::pair<float, float> GetMouseDelta();
	static float GetMouseX();
	static float GetMouseY();

private:
	enum class State
	{
		Up,
		Down,
	};

	static std::unordered_map<KeyCode, State> s_CurrentKeyStates;
	static std::unordered_map<KeyCode, State> s_PreviousKeyStates;
	static std::unordered_map<MouseButton, State> s_ButtonStates;
	static std::unordered_map<MouseButton, State> s_PreviousButtonStates;

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
