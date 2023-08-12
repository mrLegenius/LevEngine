#pragma once
#include <yaml-cpp/emitter.h>
#include <yaml-cpp/yaml.h>

#include "Scene/Entity.h"

namespace LevEngine
{
	class IComponentSerializer
	{
	public:
		virtual ~IComponentSerializer() = default;
		virtual void Serialize(YAML::Emitter& out, Entity entity) = 0;
		virtual void Deserialize(YAML::Node& node, Entity entity) = 0;
	};

	template<typename T>
	class ComponentSerializer : public IComponentSerializer
	{
	public:
		~ComponentSerializer() override = default;

		void Serialize(YAML::Emitter& out, Entity entity) override
		{
			if (!entity.HasComponent<T>()) return;

			const T& component = entity.GetComponent<T>();

			out << YAML::Key << GetKey();
			out << YAML::BeginMap;

			Serialize(out, component);

			out << YAML::EndMap;
		}

		void Deserialize(YAML::Node& node, Entity entity) override
		{
			auto componentProps = node[GetKey()];
			if (!componentProps) return;

			auto& component = entity.GetOrAddComponent<T>();
			Deserialize(componentProps, component);
		}

	protected:
		virtual const char* GetKey() = 0;
		virtual void Serialize(YAML::Emitter& out, const T& component) = 0;
		virtual void Deserialize(YAML::Node& node, T& component) = 0;
	};
}

