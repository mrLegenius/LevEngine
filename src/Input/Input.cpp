#include "Input.h"

std::unordered_map<KeyCode, Input::State> Input::s_CurrentKeyStates;
std::unordered_map<KeyCode, Input::State> Input::s_PreviousKeyStates;
std::unordered_map<MouseButton, bool> Input::s_ButtonStates;

bool Input::IsKeyPressed(const KeyCode keycode)
{
	const bool isDown = s_CurrentKeyStates[keycode] == State::Down;
	const bool stateChanged = s_CurrentKeyStates[keycode] != s_PreviousKeyStates[keycode];
	s_PreviousKeyStates[keycode] = s_CurrentKeyStates[keycode];

	return stateChanged && isDown;
}
bool Input::IsKeyReleased(const KeyCode keycode)
{
	const bool isUp = s_CurrentKeyStates[keycode] == State::Up;
	const bool stateChanged = s_CurrentKeyStates[keycode] != s_PreviousKeyStates[keycode];
	s_PreviousKeyStates[keycode] = s_CurrentKeyStates[keycode];

	return stateChanged && isUp;
}

bool Input::IsKeyDown(const KeyCode keycode)
{
	return s_CurrentKeyStates[keycode] == State::Down;
}
bool Input::IsKeyUp(const KeyCode keycode)
{
	return s_CurrentKeyStates[keycode] == State::Up;
}

bool Input::IsMouseButtonPressed(const MouseButton button)
{
	const bool isDown = IsMouseButtonDown(button);
	const bool stateChanged = s_ButtonStates[button] != isDown;
	s_ButtonStates[button] = isDown;

	return stateChanged && s_ButtonStates[button];
}
bool Input::IsMouseButtonReleased(const MouseButton button)
{
	const bool isUp = IsMouseButtonUp(button);
	const bool stateChanged = s_ButtonStates[button] != isUp;
	s_ButtonStates[button] = isUp;

	return stateChanged && !s_ButtonStates[button];
}

bool Input::IsMouseButtonDown(const MouseButton button)
{
	return false;
}
bool Input::IsMouseButtonUp(const MouseButton button)
{
	return false;
}

std::pair<float, float> Input::GetMousePosition()
{
	return { static_cast<float>(0), static_cast<float>(0) };
}

float Input::GetMouseX()
{
	auto [x, y] = GetMousePosition();
	return x;
}

float Input::GetMouseY()
{
	auto [x, y] = GetMousePosition();
	return y;
}

void Input::OnKeyPressed(const KeyCode keyCode)
{
	s_PreviousKeyStates[keyCode] = s_CurrentKeyStates[keyCode];
	s_CurrentKeyStates[keyCode] = State::Down;
}

void Input::OnKeyReleased(const KeyCode keyCode)
{
	s_PreviousKeyStates[keyCode] = s_CurrentKeyStates[keyCode];
	s_CurrentKeyStates[keyCode] = State::Up;
}
