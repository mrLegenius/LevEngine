#pragma once
#include <unordered_map>
#include "KeyCodes.h"
#include "MouseButtonCodes.h"

class Input
{
public:
	friend class Application;

	static bool IsKeyPressed(KeyCode keycode);
	static bool IsKeyReleased(KeyCode keycode);
	static bool IsKeyDown(KeyCode keycode);
	static bool IsKeyUp(KeyCode keycode);
	static bool IsMouseButtonPressed(MouseButton button);
	static bool IsMouseButtonReleased(MouseButton button);
	static bool IsMouseButtonDown(MouseButton button);
	static bool IsMouseButtonUp(MouseButton button);

	static std::pair<float, float> GetMousePosition();
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
	static std::unordered_map<MouseButton, bool> s_ButtonStates;

	static void OnKeyPressed(KeyCode keyCode);
	static void OnKeyReleased(KeyCode keyCode);
};
