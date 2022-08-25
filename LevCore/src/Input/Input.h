#pragma once
#include <unordered_map>
#include "KeyCodes.h"
#include "MouseButtonCodes.h"

namespace LevEngine
{
	class Input
	{
	public:	
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
		static std::unordered_map<KeyCode, bool> s_KeyStates;
		static std::unordered_map<MouseButton, bool> s_ButtonStates;
	};
}
