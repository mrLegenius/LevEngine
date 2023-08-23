#pragma once
#include <yaml-cpp/yaml.h>

#include "Kernel/ClassCollection.h"
#include "Scene/Entity.h"

// ReSharper disable once CppUnusedIncludeDirective
#include "Scene/Serializers/SerializerUtils.h"

namespace LevEngine
{
	class IComponentSerializer
	{
	public:
		virtual void Serialize(YAML::Emitter& out, Entity entity) = 0;
		virtual void Deserialize(YAML::Node& node, Entity entity) = 0;
	};

	template<class TComponent, class TSerializer>
	class ComponentSerializer : public IComponentSerializer
	{
	public:
		void Serialize(YAML::Emitter& out, Entity entity) override
		{
			if (!entity.HasComponent<TComponent>()) return;

			const TComponent& component = entity.GetComponent<TComponent>();

			out << YAML::Key << GetKey();
			out << YAML::BeginMap;

			SerializeData(out, component);

			out << YAML::EndMap;
		}

		void Deserialize(YAML::Node& node, Entity entity) override
		{
			auto componentProps = node[GetKey()];
			if (!componentProps) return;

			auto& component = entity.GetOrAddComponent<TComponent>();
			DeserializeData(componentProps, component);
		}

	protected:
		virtual const char* GetKey() = 0;
		virtual void SerializeData(YAML::Emitter& out, const TComponent& component) = 0;
		virtual void DeserializeData(YAML::Node& node, TComponent& component) = 0;

	private:
		static inline ClassRegister<IComponentSerializer, TSerializer> s_ClassRegister;
	};
}

