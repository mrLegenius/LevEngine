#pragma once
#include "imguizmo/ImGuizmo.h"

namespace LevEngine
{
	class Gizmo
	{
	public:
		enum class ToolType
		{
			None = 0,
			Translate = ImGuizmo::OPERATION::TRANSLATE,
			Rotate = ImGuizmo::OPERATION::ROTATE,
			Scale = ImGuizmo::OPERATION::SCALE,
		};

		static inline ToolType Tool = ToolType::None;
	};
}
