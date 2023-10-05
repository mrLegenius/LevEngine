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

		enum class ToolSpace
		{
			World = ImGuizmo::WORLD,
			Local = ImGuizmo::LOCAL,
		};

		static inline ToolType Tool = ToolType::None;
		static inline ToolSpace Space = ToolSpace::World;
	};
}
