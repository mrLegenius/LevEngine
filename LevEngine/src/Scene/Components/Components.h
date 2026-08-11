#pragma once

#include "Kernel/Core.h"

namespace LevEngine
{
	struct LEV_API IDComponent
	{
		UUID ID;

		IDComponent(const UUID& ID) : ID(ID){};
		IDComponent(const IDComponent&) = default;
	};

	struct LEV_API TagComponent
	{
		String tag;

		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		explicit TagComponent(String other)
			: tag(Move(other)) { }
	};
}
