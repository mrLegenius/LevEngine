#include "levpch.h"
#include "Input.h"
#include "Math/Vector2.h"

namespace LevEngine
{
UnorderedMap<KeyCode, Input::State> Input::s_CurrentKeyStates;
UnorderedMap<KeyCode, Input::State> Input::s_PreviousKeyStates;
UnorderedMap<MouseButton, Input::State> Input::s_ButtonStates;
UnorderedMap<MouseButton, Input::State> Input::s_PreviousButtonStates;
float Input::s_MousePositionX;
float Input::s_MousePositionY;
float Input::s_MousePreviousX;
float Input::s_MousePreviousY;
float Input::s_MouseWheelOffset;

bool Input::IsKeyPressed(const KeyCode keycode)
{
	const bool isDown = s_CurrentKeyStates[keycode] == State::Down;
	const bool stateChanged = s_CurrentKeyStates[keycode] != s_PreviousKeyStates[keycode];

	return stateChanged && isDown;
}
bool Input::IsKeyReleased(const KeyCode keycode)
{
	const bool isUp = s_CurrentKeyStates[keycode] == State::Up;
	const bool stateChanged = s_CurrentKeyStates[keycode] != s_PreviousKeyStates[keycode];

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
	const bool stateChanged = s_ButtonStates[button] != s_PreviousButtonStates[button];

	return stateChanged && isDown;
}
bool Input::IsMouseButtonReleased(const MouseButton button)
{
	const bool isUp = s_ButtonStates[button] == State::Up;
	const bool stateChanged = s_ButtonStates[button] != s_PreviousButtonStates[button];

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

float Input::GetMouseWheelOffset()
{
	return s_MouseWheelOffset;
}

Vector2 Input::GetMousePosition()
{
	return { s_MousePositionX, s_MousePositionY };
}

Vector2 Input::GetMouseDelta()
{
	return { s_MousePositionX - s_MousePreviousX, s_MousePositionY - s_MousePreviousY };
}

float Input::GetMouseX()
{
	return s_MousePositionX;
}

float Input::GetMouseY()
{
	return s_MousePositionY;
}

void Input::CreateLuaBind(sol::state& lua)
{
	lua.new_usertype<Input>(
		"Input",
		sol::no_constructor,
		"getMouseDelta", []()
		{
			return GetMouseDelta();
		}
	);
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
	s_MousePreviousX = s_MousePositionX;
	s_MousePreviousY = s_MousePositionY;

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

void Input::OnMouseScrolled(const float offset)
{
	s_MouseWheelOffset = offset;
}

void Input::Reset()
{
	for (auto e : s_PreviousKeyStates)
		s_PreviousKeyStates[e.first] = s_CurrentKeyStates[e.first];

	for (auto e : s_PreviousButtonStates)
		s_PreviousButtonStates[e.first] = s_ButtonStates[e.first];

	s_MouseWheelOffset = 0;
	s_MousePreviousX = s_MousePositionX;
	s_MousePreviousY = s_MousePositionY;
}
}
