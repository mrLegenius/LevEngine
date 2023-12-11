#include "levpch.h"
#include "SceneSerializer.h"

#include "../Entity.h"
#include "Assets/ScriptAsset.h"
#include "Kernel/ClassCollection.h"
#include "Scene/Scene.h"
#include "Scene/Components/ComponentSerializer.h"
#include "Scene/Components/Transform/Transform.h"

namespace LevEngine
{
	SceneSerializer::SceneSerializer(const Ref<Scene>& scene)
		: m_Scene(scene)
	{
	}

	void SceneSerializer::SerializeEntities(YAML::Emitter& out) const
	{
		out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;

		m_Scene->ForEachEntity(
			[&](auto entity)
			{
				SerializeEntity(out, entity);
			});

		out << YAML::EndSeq;
	}

	void SceneSerializer::SerializeScriptSystems(YAML::Emitter& out) const
	{
		out << YAML::Key << "Systems" << YAML::Value << YAML::BeginSeq;

		auto scriptAssets = m_Scene->GetActiveScriptSystems();

		for (const auto & scriptAsset : scriptAssets)
		{
			out << YAML::BeginMap;
			out << YAML::Key << "System" << YAML::Value << scriptAsset->GetUUID();
			out << YAML::EndMap;
		}
		
		out << YAML::EndSeq;
	}

	void SceneSerializer::Serialize(const Path& filepath) const
	{
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Scene" << YAML::Value << filepath.stem().string();
		
		SerializeEntities(out);
		
		SerializeScriptSystems(out);
		
		out << YAML::EndMap;

		std::ofstream fout(filepath.c_str());
		fout << out.c_str();
	}

	void SceneSerializer::DeserializeEntities(const YAML::Node& data) const
	{
		if (auto entities = data["Entities"])
		{
			std::unordered_map<UUID, Entity> entitiesMap;
			std::unordered_map<UUID, Pair<Entity, YAML::Node>> entitiesToDeserialize;
			std::unordered_map<UUID, UUID> relationships;

			for (auto entity : entities)
			{
				auto uuid = entity["Entity"].as<uint64_t>();
				auto name = entity["Tag"].as<String>();

				if (auto parent = entity["Parent"])
				{
					auto parentUuid = UUID(parent.as<uint64_t>());
					relationships.emplace(uuid, parentUuid);
				}

				Log::Trace("Deserializing entity with ID = {0}, name = {1}", uuid, name);

				Entity deserializedEntity = m_Scene->CreateEntity(uuid, name);

				entitiesToDeserialize.try_emplace(uuid, Pair<Entity, YAML::Node>(deserializedEntity, entity));
				entitiesMap.try_emplace(uuid, deserializedEntity);
			}

			for (auto& [uuid, pair] : entitiesToDeserialize)
			{
				auto deserializedEntity = pair.first;
				auto entityNode = pair.second;
				for (const auto serializer : ClassCollection<IComponentSerializer>::Instance())
					serializer->Deserialize(entityNode, deserializedEntity);
			}

			for (auto& [uuid, entity] : entitiesMap)
			{
				if (!entity) continue;

				auto& transform = entity.GetComponent<Transform>();
				transform.SetParent(entitiesMap[relationships[uuid]], false);
			}
		}
	}

	void SceneSerializer::DeserializeScriptSystems(const YAML::Node& data) const
	{
		if (auto systems = data["Systems"])
		{
			for (const auto& system : systems)
			{
				auto uuid = system["System"].as<uint64_t>();

				if (auto script = AssetDatabase::GetAsset<ScriptAsset>(uuid))
				{
					m_Scene->SetScriptSystemActive(script, true);
				}
			}
		}
	}

	bool SceneSerializer::Deserialize(const String& filepath) const
	{
		YAML::Node data;
		if (!LoadYAMLFileSafe(filepath.c_str(), data))
		{
			Log::CoreError("Failed to load scene data");
			return false;
		}

		if (!data["Scene"])
			return false;

		auto sceneName = data["Scene"].as<String>();
		Log::CoreTrace("Deserializing scene '{0}'", sceneName);

		DeserializeEntities(data);

		DeserializeScriptSystems(data);
		
		return true;
	}

	void SceneSerializer::SerializeRuntime(const String& filepath)
	{
		LEV_NOT_IMPLEMENTED
	}

	bool SceneSerializer::DeserializeRuntime(const String& filepath)
	{
		LEV_NOT_IMPLEMENTED
	}
}
