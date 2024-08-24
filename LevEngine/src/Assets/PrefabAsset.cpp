#include "levpch.h"
#include "PrefabAsset.h"

#include "JobSystem/ParallelJob.h"
#include "Physics/Components/CharacterController.h"
#include "Physics/Components/Rigidbody.h"
#include "Scene/Scene.h"
#include "Scene/Components/Components.h"
#include "Scene/Components/ComponentSerializer.h"
#include "Scene/Components/Transform/Transform.h"
#include "Scene/Components/Transform/TransformSerializer.h"

namespace LevEngine
{
    Entity PrefabAsset::Instantiate(const Ref<Scene>& scene)
    {
        if (!m_DeserializedData.IsDefined())
        {
            Log::CoreError("Failed to instantiate a prefab. Data is corrupted in {0}", m_Path);
            return {};
        }
        
        auto prefabName = m_DeserializedData["Prefab"].as<String>();
        Log::CoreTrace("Instantiating '{0}'...", prefabName);

        auto rootEntity = m_DeserializedData["RootEntity"].as<uint64_t>();

        auto entities = m_DeserializedData["Entities"];
        if (!entities)
        {
            Log::CoreError("Failed to instantiate a prefab. There is no entities", m_Path);
            return {};  
        }

        std::unordered_map<UUID, Entity> entitiesMap;
        std::unordered_map<UUID, UUID> relationships;
        Vector<Pair<Entity, const YAML::Node>> entitiesToDeserialize;
        
        for (auto entity : entities)
        {
            auto uuid = entity["Entity"].as<uint64_t>();
            auto name = entity["Tag"].as<String>();
            
            if (auto parent = entity["Parent"])
            {
                auto parentUuid = UUID(parent.as<uint64_t>());
                relationships.emplace(uuid, parentUuid);
            }

            const Entity deserializedEntity = scene->CreateEntity(uuid, name);

            TransformSerializer::DeserializeData(entity, deserializedEntity.GetComponent<Transform>());

            entitiesToDeserialize.emplace_back(Pair<Entity, const YAML::Node>(deserializedEntity, entity));
            entitiesMap.try_emplace(uuid, deserializedEntity);
        }
        
        for (auto& [uuid, entity] : entitiesMap)
        {
            if (!entity) continue;

            auto& transform = entity.GetComponent<Transform>();
            transform.SetParent(entitiesMap[relationships[uuid]], false);
        }

        ParallelJob serializeJob([=](const int i)
        {
            auto [deserializedEntity, entityNode] = entitiesToDeserialize[i];
            for (const auto serializer : ClassCollection<IComponentSerializer>::Instance())
                serializer->Deserialize(entityNode, deserializedEntity);
        });

        serializeJob.Schedule(entitiesToDeserialize.size());
        serializeJob.Wait();

        for (auto& [uuid, entity] : entitiesMap)
        {
            if (!entity) continue;
            entity.AddOrReplaceComponent<IDComponent>(UUID{});
        }

        return entitiesMap[rootEntity];
    }

    Entity PrefabAsset::Instantiate(const Ref<Scene>& scene, const Entity parent)
    {
        const auto instance = Instantiate(scene);
        if (parent)
            instance.GetComponent<Transform>().SetParent(parent);

        return instance;
    }

    Entity PrefabAsset::Instantiate(const Ref<Scene>& scene, const Vector3 position)
    {
        const auto instance = Instantiate(scene);
        
        if (instance.HasComponent<Rigidbody>())
            instance.GetComponent<Rigidbody>().Teleport(position);
        else if (instance.HasComponent<CharacterController>())
            instance.GetComponent<CharacterController>().Teleport(position);
        else
            instance.GetComponent<Transform>().SetWorldPosition(position);
        
        return instance;
    }

    void PrefabAsset::SaveEntity(const Entity entity)
    {
        LEV_CORE_ASSERT(entity.HasComponent<IDComponent>());

        m_EntityToSave = entity;
        Serialize();
    }

    void PrefabAsset::SerializeData(YAML::Emitter& out)
    {
        const auto entity = m_EntityToSave;

        if (!entity) return;

        out << YAML::Key << "Prefab" << YAML::Value << m_Name;
        out << YAML::Key << "RootEntity" << YAML::Value << entity.GetUUID();
        Queue<Entity> entitiesToSerialize;
        entitiesToSerialize.push(entity);
        
        out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;
        while (!entitiesToSerialize.empty())
        {
            auto currentEntity = entitiesToSerialize.front();
            entitiesToSerialize.pop();
            
            SerializeEntity(out, currentEntity);

            for (auto child : currentEntity.GetComponent<Transform>().GetChildren())
                entitiesToSerialize.push(child);
        }
        out << YAML::EndSeq;
    }
}
