#include "Input.h"

std::unordered_map<KeyCode, Input::State> Input::s_CurrentKeyStates;
std::unordered_map<KeyCode, Input::State> Input::s_PreviousKeyStates;
std::unordered_map<MouseButton, Input::State> Input::s_ButtonStates;
std::unordered_map<MouseButton, Input::State> Input::s_PreviousButtonStates;
float Input::s_MousePositionX;
float Input::s_MousePositionY;

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
	const bool isDown = s_PreviousButtonStates[button] == State::Down;
	const bool stateChanged = s_PreviousButtonStates[button] != s_PreviousButtonStates[button];
	s_PreviousButtonStates[button] = s_PreviousButtonStates[button];

	return stateChanged && isDown;
}
bool Input::IsMouseButtonReleased(const MouseButton button)
{
	const bool isUp = s_ButtonStates[button] == State::Up;
	const bool stateChanged = s_ButtonStates[button] != s_PreviousButtonStates[button];
	s_PreviousButtonStates[button] = s_ButtonStates[button];

	return stateChanged && isUp;
}

bool Input::IsMouseButtonDown(const MouseButton button)
{
	return s_ButtonStates[button] == State::Down;
}
bool Input::IsMouseButtonUp(const MouseButton button)
{
	return s_ButtonStates[button] == State::Up;
}

std::pair<float, float> Input::GetMousePosition()
{
	return { s_MousePositionX, s_MousePositionY };
}

float Input::GetMouseX()
{
	return s_MousePositionX;
}

float Input::GetMouseY()
{
	return s_MousePositionY;
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

void Input::OnMouseMoved(const float x, const float y)
{
	s_MousePositionX += x;
	s_MousePositionY += y;
}

void Input::OnMouseButtonPressed(const MouseButton mouseButton)
{
	s_PreviousButtonStates[mouseButton] = s_ButtonStates[mouseButton];
	s_ButtonStates[mouseButton] = State::Down;
}


void Input::OnMouseButtonReleased(const MouseButton mouseButton)
{
	s_PreviousButtonStates[mouseButton] = s_ButtonStates[mouseButton];
	s_ButtonStates[mouseButton] = State::Up;
}