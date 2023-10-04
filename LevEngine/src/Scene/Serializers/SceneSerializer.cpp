#include "levpch.h"
#include "SceneSerializer.h"

#include "../Entity.h"
#include "Kernel/ClassCollection.h"
#include "Scene/Components/ComponentSerializer.h"

namespace LevEngine
{
	SceneSerializer::SceneSerializer(const Ref<Scene>& scene)
		: m_Scene(scene)
	{
	}

	void SceneSerializer::Serialize(const Path& filepath) const
	{
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Scene" << YAML::Value << filepath.stem().string();
		out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;

		m_Scene->ForEachEntity(
			[&](auto entity)
			{
				SerializeEntity(out, entity);
			});

		out << YAML::EndSeq;
		out << YAML::EndMap;

		std::ofstream fout(filepath.c_str());
		fout << out.c_str();
	}

	bool SceneSerializer::Deserialize(const String& filepath)
	{
		YAML::Node data;
		try
		{
			data = YAML::LoadFile(filepath.c_str());
		}
		catch (std::exception& e)
		{
			Log::CoreWarning(e.what());
			return false;
		}

		if (!data["Scene"])
			return false;

		auto sceneName = data["Scene"].as<String>();
		Log::CoreTrace("Deserializing scene '{0}'", sceneName);

		auto entities = data["Entities"];
		if (!entities) return true;

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
