#pragma once

#include "Kernel/Core.h"

namespace LevEngine
{
	//A small native window shown while the engine and the client layers are still initializing.
	//It does not depend on the render device, ImGui or the game loop, so it can be brought up
	//before anything else and torn down as soon as the first frame is about to be rendered.
	//Every call is a no-op while the splash screen is not shown, so status reports may stay
	//in code that also runs later at runtime.
	class LEV_API SplashScreen
	{
	public:
		static void Show(const String& title, const Path& logoPath);
		//Replaces the line describing what is being loaded and repaints immediately
		static void SetStatus(const String& status);
		static void Hide();
		[[nodiscard]] static bool IsShown();
	};
}
