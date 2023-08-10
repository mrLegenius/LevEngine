#pragma once
#include "Scene/Entity.h"

namespace LevEngine
{
	struct GUIUtils
	{
		static void DrawVector3Control(const std::string& label, Vector3& values, float resetValue = 0.0f, float columnWidth = 100.0f);
	};

}
