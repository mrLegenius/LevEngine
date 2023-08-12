#pragma once

#include "ComponentSerializer.h"

namespace LevEngine
{
	class TagSerializer final : public ComponentSerializer<TagComponent>
	{
	public:

		//TODO: Why can't I call it from inherited class directly?
		void Serialize(YAML::Emitter& out, const Entity entity) override
		{
			return ComponentSerializer::Serialize(out, entity);
		}

		std::string Deserialize(YAML::Node& node)
		{
			if (auto props = node[GetKey()])
				return props["Tag"].as<std::string>();

			return {};
		}
	protected:
		const char* GetKey() override { return "Tag"; }

		void Serialize(YAML::Emitter& out, const TagComponent& component) override
		{
			out << YAML::Key << "Tag" << YAML::Value << component.tag;
		}
		void Deserialize(YAML::Node& node, TagComponent& component) override
		{
			LEV_THROW("There is custom deserialization of tag")
		}
	};
}
