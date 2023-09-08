#include "levpch.h"
#include "SceneSerializer.h"

#include "../Entity.h"
#include "../Components/Components.h"
#include "Kernel/ClassCollection.h"
#include "Scene/Components/ComponentSerializer.h"

namespace LevEngine
{
	SceneSerializer::SceneSerializer(const Ref<Scene>& scene)
		: m_Scene(scene)
	{
	}

	void SceneSerializer::SerializeEntity(YAML::Emitter& out, Entity entity) const
	{
		LEV_CORE_ASSERT(entity.HasComponent<IDComponent>());

		out << YAML::BeginMap;

		out << YAML::Key << "Entity" << YAML::Value << entity.GetUUID();
		out << YAML::Key << "Tag" << YAML::Value << entity.GetComponent<TagComponent>().tag.c_str();

		if (auto parent = entity.GetComponent<Transform>().GetParent())
			out << YAML::Key << "Parent" << YAML::Value << parent.GetUUID();

		for (const auto serializer : ClassCollection<IComponentSerializer>::Instance())
			serializer->Serialize(out, entity);

		out << YAML::EndMap;
	}

	void SceneSerializer::Serialize(const String& filepath) const
	{
		YAML::Emitter out;
		out << YAML::BeginMap;
		//TODO: Put actual scene name
		out << YAML::Key << "Scene" << YAML::Value << "Scene Name";
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
		Log::Trace("Deserializing scene '{0}'", sceneName);

		auto entities = data["Entities"];
		if (!entities) return true;

		UnorderedMap<UUID, Entity> entitiesMap;
		UnorderedMap<UUID, UUID> relationships;

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

			for (const auto serializer : ClassCollection<IComponentSerializer>::Instance())
				serializer->Deserialize(entity, deserializedEntity);

			entitiesMap.try_emplace(uuid, deserializedEntity);
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
