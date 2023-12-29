#pragma once

#include "Kernel/ClassCollection.h"
#include "Scene/Entity.h"

// ReSharper disable once CppUnusedIncludeDirective
#include "Scene/Serializers/SerializerUtils.h"

namespace LevEngine
{
	class IComponentSerializer
	{
	public:
		virtual ~IComponentSerializer() = default;
		virtual void Serialize(YAML::Emitter& out, Entity entity) = 0;
		virtual void Deserialize(const YAML::Node& node, Entity entity) = 0;
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

		void Deserialize(const YAML::Node& node, Entity entity) override
		{
			const auto key = GetKey();
			const auto& componentProps = node[key];
			if (!componentProps) return;

			if (!entity.HasComponent<TComponent>())
			{
				TComponent component = TComponent();
				DeserializeData(componentProps, component);
				entity.AddComponent<TComponent>(component);
			}
			else
			{
				auto& component = entity.GetComponent<TComponent>();
				DeserializeData(componentProps, component);
			}			
		}

	protected:
		virtual const char* GetKey() = 0;
		virtual void SerializeData(YAML::Emitter& out, const TComponent& component) = 0;
		virtual void DeserializeData(const YAML::Node& node, TComponent& component) = 0;

	private:
		static inline ClassRegister<IComponentSerializer, TSerializer> s_ClassRegister;
	};
}

