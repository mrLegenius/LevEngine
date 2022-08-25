#include <GLFW/glfw3.h>

#include "Kernel/Application.h"
#include "Input/Input.h"

namespace LevEngine
{
	std::unordered_map<KeyCode, bool> Input::s_KeyStates;
	std::unordered_map<MouseButton, bool> Input::s_ButtonStates;
	
	bool Input::IsKeyPressed(const KeyCode keycode)
	{
		const bool isDown = IsKeyDown(keycode);
		const bool stateChanged = s_KeyStates[keycode] != isDown;
		s_KeyStates[keycode] = isDown;

		return stateChanged && s_KeyStates[keycode];
	}
	bool Input::IsKeyReleased(const KeyCode keycode)
	{
		const bool isUp = IsKeyUp(keycode);
		const bool stateChanged = s_KeyStates[keycode] != isUp;
		s_KeyStates[keycode] = isUp;

		return stateChanged && !s_KeyStates[keycode];
	}

	bool Input::IsKeyDown(const KeyCode keycode)
	{
		const auto window = static_cast<GLFWwindow*>(
			Application::Get().GetWindow().GetNativeWindow());

		const auto state = glfwGetKey(window, static_cast<int>(keycode));
		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}
	bool Input::IsKeyUp(const KeyCode keycode)
	{
		const auto window = static_cast<GLFWwindow*>(
			Application::Get().GetWindow().GetNativeWindow());

		const auto state = glfwGetKey(window, static_cast<int>(keycode));
		return state == GLFW_RELEASE;
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
		const auto window = static_cast<GLFWwindow*>(
			Application::Get().GetWindow().GetNativeWindow());

		const auto state = glfwGetMouseButton(window, static_cast<int>(button));
		return state == GLFW_PRESS;
	}
	bool Input::IsMouseButtonUp(const MouseButton button)
	{
		const auto window = static_cast<GLFWwindow*>(
			Application::Get().GetWindow().GetNativeWindow());

		const auto state = glfwGetMouseButton(window, static_cast<int>(button));
		return state == GLFW_RELEASE;
	}
	

	std::pair<float, float> Input::GetMousePosition()
	{
		const auto window = static_cast<GLFWwindow*>(
			Application::Get().GetWindow().GetNativeWindow());

		double xPos, yPos;
		glfwGetCursorPos(window, &xPos, &yPos);
		return { static_cast<float>(xPos), static_cast<float>(yPos) };
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
}
