#pragma once

#include "Kernel/Core.h"

#include "Assets/MissingReferences.h"
#include "Kernel/ClassCollection.h"
#include "Scene/Entity.h"

// ReSharper disable once CppUnusedIncludeDirective
#include "Scene/Serializers/SerializerUtils.h"

namespace LevEngine
{
	class LEV_API IComponentSerializer
	{
	public:
		virtual ~IComponentSerializer() = default;
		virtual void Serialize(YAML::Emitter& out, Entity entity) = 0;
		virtual void Deserialize(const YAML::Node& node, Entity entity) = 0;

		// The serializer already knows a component's name and how to build a default one, which is
		// everything needed to add, remove or find a component without naming its type at compile
		// time. Used by the editor's agent bridge, see LevEditor/src/Agent.
		virtual const char* GetComponentKey() = 0;
		virtual bool HasComponent(Entity entity) = 0;
		virtual bool AddComponent(Entity entity) = 0;
		virtual bool RemoveComponent(Entity entity) = 0;
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

			//<--- Names the component an asset reference was read from, see MissingReferences ---<<
			const MissingReferences::LocationScope scope(key);

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

		const char* GetComponentKey() override { return GetKey(); }

		bool HasComponent(Entity entity) override { return entity.HasComponent<TComponent>(); }

		bool AddComponent(Entity entity) override
		{
			if (entity.HasComponent<TComponent>()) return false;

			//<--- Defaults come from the component's own constructor rather than from a guess
			//at what its YAML looks like ---<<
			entity.AddComponent<TComponent>(TComponent());
			return true;
		}

		bool RemoveComponent(Entity entity) override
		{
			if (!entity.HasComponent<TComponent>()) return false;

			entity.RemoveComponent<TComponent>();
			return true;
		}

	protected:
		virtual const char* GetKey() = 0;
		virtual void SerializeData(YAML::Emitter& out, const TComponent& component) = 0;
		virtual void DeserializeData(const YAML::Node& node, TComponent& component) = 0;

	private:
		static inline ClassRegister<IComponentSerializer, TSerializer> s_ClassRegister;
	};
}

