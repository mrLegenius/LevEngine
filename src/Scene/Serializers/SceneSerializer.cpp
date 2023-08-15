#include "pch.h"
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
		out << YAML::Key << "Tag" << YAML::Value << entity.GetComponent<TagComponent>().tag;

		for (const auto serializer : ClassCollection<IComponentSerializer>::Instance())
			serializer->Serialize(out, entity);

		out << YAML::EndMap;
	}

	void SceneSerializer::Serialize(const std::string& filepath) const
	{
		YAML::Emitter out;
		out << YAML::BeginMap;
		//TODO: Put actual scene name
		out << YAML::Key << "Scene" << YAML::Value << "Scene Name";
		out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;

		//TODO: Restore all entities parent/children relationship
		m_Scene->ForEachEntity(
			[&](auto entity)
			{
				SerializeEntity(out, entity);
			});

		out << YAML::EndSeq;
		out << YAML::EndMap;

		std::ofstream fout(filepath);
		fout << out.c_str();
	}

	bool SceneSerializer::Deserialize(const std::string& filepath)
	{
		YAML::Node data;
		try
		{
			data = YAML::LoadFile(filepath);
		}
		catch (std::exception& e)
		{
			Log::CoreWarning(e.what());
			return false;
		}

		if (!data["Scene"])
			return false;

		auto sceneName = data["Scene"].as<std::string>();
		Log::Trace("Deserializing scene '{0}'", sceneName);

		auto entities = data["Entities"];
		if (!entities) return true;

		for (auto entity : entities)
		{
			auto uuid = entity["Entity"].as<uint64_t>();
			auto name = entity["Tag"].as<std::string>();

			Log::Trace("Deserializing entity with ID = {0}, name = {1}", uuid, name);

			Entity deserializedEntity = m_Scene->CreateEntity(uuid, name);

			for (const auto serializer : ClassCollection<IComponentSerializer>::Instance())
				serializer->Deserialize(entity, deserializedEntity);
		}

		return true;
	}

	void SceneSerializer::SerializeRuntime(const std::string& filepath)
	{
		LEV_NOT_IMPLEMENTED
	}

	bool SceneSerializer::DeserializeRuntime(const std::string& filepath)
	{
		LEV_NOT_IMPLEMENTED
	}
}
