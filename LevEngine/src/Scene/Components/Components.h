#pragma once
#include "DataTypes/Utility.h"
#include "Kernel/UUID.h"
#include "DataTypes/String.h"

namespace LevEngine
{
	struct IDComponent
	{
		UUID ID;

		IDComponent(const UUID& ID) : ID(ID){};
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
