#include "pch.h"
#include "EntitySnapshot.h"

#include "Kernel/ClassCollection.h"
#include "Scene/Components/Destroyable.h"
#include "Scene/Components/ComponentSerializer.h"
#include "Scene/Components/Transform/Transform.h"
#include "Scene/Components/Transform/TransformSerializer.h"
#include "Scene/Serializers/SerializerUtils.h"

namespace LevEngine::Editor
{
    namespace
    {
        void CollectHierarchy(const Entity entity, Vector<Entity>& entities)
        {
            if (!entity) return;

            entities.push_back(entity);

            //<--- Copied, restoring reorders the children of whatever it touches ---<<
            const Vector<Entity> children = entity.GetComponent<Transform>().GetChildren();
            for (const auto child : children)
                CollectHierarchy(child, entities);
        }

        void RestoreComponents(const YAML::Node& node, const Entity entity)
        {
            for (const auto& serializer : ClassCollection<IComponentSerializer>::Instance())
            {
                if (node[serializer->GetComponentKey()])
                    serializer->Deserialize(node, entity);
                else
                    //<--- Not in the snapshot means it was added after it was taken ---<<
                    serializer->RemoveComponent(entity);
            }
        }

        void RestoreTransform(const YAML::Node& node, const Entity entity, const Ref<Scene>& scene)
        {
            auto& transform = entity.GetComponent<Transform>();

            TransformSerializer::DeserializeData(node, transform);

            //<--- SetParent appends, so the place among the siblings is restored afterwards ---<<
            const auto childIndex = transform.GetChildIndex();

            Entity parent = scene->GetRootEntity();

            if (const auto parentNode = node["Parent"])
            {
                if (const Entity found = scene->GetEntityByUUID(UUID(parentNode.as<uint64_t>())))
                    parent = found;
            }

            if (transform.GetParent() != parent)
                transform.SetParent(parent, false);

            transform.SetChildIndex(childIndex);

            if (const auto currentParent = transform.GetParent())
                currentParent.GetComponent<Transform>().SortChildren();

            transform.RecalculateModel();
        }
    }

    String EntitySnapshot::Capture(const Entity entity)
    {
        if (!entity) return {};

        YAML::Emitter out;
        SerializeEntity(out, entity);

        return String{ out.c_str() };
    }

    String EntitySnapshot::CaptureHierarchy(const Entity entity)
    {
        if (!entity) return {};

        Vector<Entity> entities;
        CollectHierarchy(entity, entities);

        YAML::Emitter out;
        out << YAML::BeginSeq;

        for (const auto branchEntity : entities)
            SerializeEntity(out, branchEntity);

        out << YAML::EndSeq;

        return String{ out.c_str() };
    }

    void EntitySnapshot::Restore(const Entity entity, const String& snapshot)
    {
        if (!entity || snapshot.empty()) return;

        const auto& scene = SceneManager::GetActiveScene();
        if (!scene) return;

        const YAML::Node node = YAML::Load(snapshot.c_str());
        if (!node || !node.IsMap()) return;

        if (const auto tag = node["Tag"])
            entity.GetComponent<TagComponent>().tag = tag.as<String>();

        RestoreTransform(node, entity, scene);
        RestoreComponents(node, entity);
    }

    Entity EntitySnapshot::RestoreHierarchy(const Ref<Scene>& scene, const String& snapshot)
    {
        if (!scene || snapshot.empty()) return {};

        const YAML::Node nodes = YAML::Load(snapshot.c_str());
        if (!nodes || !nodes.IsSequence()) return {};

        Vector<Pair<Entity, YAML::Node>> restored;

        for (const auto& node : nodes)
        {
            const auto uuid = UUID(node["Entity"].as<uint64_t>());
            const auto tag = node["Tag"].as<String>();

            Entity entity = scene->GetEntityByUUID(uuid);

            if (!entity)
                entity = scene->CreateEntity(uuid, tag);
            else
                entity.GetComponent<TagComponent>().tag = tag;

            //<--- Destruction is deferred, so an entity undone and redone inside one frame is still
            //there and still marked. Taking the mark off is what keeps it alive ---<<
            if (entity.HasComponent<Destroyable>())
                entity.RemoveComponent<Destroyable>();

            restored.emplace_back(entity, node);
        }

        //<--- Parents are all there by now, whether they came from the snapshot or were never gone ---<<
        for (const auto& [entity, node] : restored)
        {
            RestoreTransform(node, entity, scene);
            RestoreComponents(node, entity);
        }

        return restored.empty() ? Entity{} : restored.front().first;
    }
}
