#pragma once

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
		std::string tag;

		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		explicit TagComponent(std::string other)
			: tag(std::move(other)) { }
	};
}
