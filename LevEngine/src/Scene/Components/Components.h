#pragma once
#include "DataTypes/Utility.h"

namespace LevEngine
{
	struct IDComponent
	{
		UUID ID;

		IDComponent() = default;
		IDComponent(const IDComponent&) = default;
	};

	struct TagComponent
	{
		String tag;

		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		explicit TagComponent(String other)
			: tag(Move(other)) { }
	};
}
