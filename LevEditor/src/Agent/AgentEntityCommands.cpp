#include "pch.h"
#include "AgentBridge.h"

#include "AgentJson.h"

#include "Kernel/ClassCollection.h"
#include "Scene/Components/ComponentSerializer.h"
#include "Scene/Components/Transform/Transform.h"
#include "Scene/Components/Transform/TransformSerializer.h"
#include "Undo/UndoCommands.h"
#include "Undo/UndoSystem.h"

namespace LevEngine::Editor
{
    namespace
    {
        //<--- Transform is not a registered serializer -- every entity has one and the scene file
        //writes it inline -- so it is handled by hand wherever a component key is expected ---<<
        constexpr const char* k_TransformKey = "Transform";

        Ref<IComponentSerializer> FindSerializer(const String& key)
        {
            for (const auto& serializer : ClassCollection<IComponentSerializer>::Instance())
            {
                if (key == serializer->GetComponentKey())
                    return serializer;
            }

            return nullptr;
        }

        //<--- Everything an entity holds, as one YAML map, which is what the scene file would get ---<<
        YAML::Node SerializeEntity(const Entity entity)
        {
            YAML::Emitter out;
            out << YAML::BeginMap;

            for (const auto& serializer : ClassCollection<IComponentSerializer>::Instance())
                serializer->Serialize(out, entity);

            out << YAML::EndMap;

            return YAML::Load(out.c_str());
        }

        bool LooksLikeNumber(const std::string& text)
        {
            if (text.empty()) return false;

            char* end = nullptr;
            std::strtod(text.c_str(), &end);

            return end != nullptr && *end == '\0';
        }

        void WriteYamlAsJson(JsonWriter& writer, const YAML::Node& node)
        {
            switch (node.Type())
            {
            case YAML::NodeType::Map:
                writer.BeginObject();
                for (const auto& entry : node)
                {
                    writer.Key(String{ entry.first.Scalar().c_str() });
                    WriteYamlAsJson(writer, entry.second);
                }
                writer.EndObject();
                break;

            case YAML::NodeType::Sequence:
                writer.BeginArray();
                for (const auto& entry : node)
                    WriteYamlAsJson(writer, entry);
                writer.EndArray();
                break;

            case YAML::NodeType::Scalar:
                {
                    const std::string& text = node.Scalar();

                    if (text == "true") writer.Value(true);
                    else if (text == "false") writer.Value(false);
                    else if (LooksLikeNumber(text)) writer.Value(std::strtod(text.c_str(), nullptr));
                    else writer.Value(String{ text.c_str() });
                }
                break;

            default:
                writer.Null();
                break;
            }
        }

        // Only the fields that were given are changed. A component is written back through its own
        // deserializer, which needs the whole thing, so what is on the entity now is what the missing
        // fields come from.
        YAML::Node MergeNodes(const YAML::Node& base, const YAML::Node& overrides)
        {
            if (!overrides || !overrides.IsDefined()) return base;

            if (!base || !base.IsMap() || !overrides.IsMap())
                return overrides;

            YAML::Node result = YAML::Clone(base);

            for (const auto& entry : overrides)
            {
                const auto key = entry.first.Scalar();
                result[key] = MergeNodes(result[key], entry.second);
            }

            return result;
        }

        //<--- Everything that is not naming the entity or the component is taken as a value, so
        //both {"values":{"Radius":10}} and {"Radius":10} work ---<<
        YAML::Node CollectValues(const YAML::Node& arguments)
        {
            if (arguments["values"]) return arguments["values"];

            YAML::Node values{ YAML::NodeType::Map };

            if (!arguments || !arguments.IsMap()) return values;

            for (const auto& entry : arguments)
            {
                const auto key = entry.first.Scalar();

                if (key == "entity" || key == "id" || key == "uuid" || key == "tag" || key == "component")
                    continue;

                values[key] = entry.second;
            }

            return values;
        }

        void WriteTransform(JsonWriter& writer, const Transform& transform)
        {
            writer.BeginObject();
            writer.KeyVector3("position", transform.GetLocalPosition());
            writer.KeyVector3("rotation", transform.GetLocalRotation().ToEuler() * Math::RadToDeg);
            writer.KeyVector3("scale", transform.GetLocalScale());
            writer.KeyVector3("worldPosition", transform.GetWorldPosition());
            writer.KeyVector3("worldRotation", transform.GetWorldRotation().ToEuler() * Math::RadToDeg);
            writer.KeyVector3("forward", transform.GetForwardDirection());

            if (const auto parent = transform.GetParent())
                writer.KeyValue("parent", static_cast<uint64_t>(parent.GetUUID()));
            else
                writer.Key("parent").Null();

            writer.EndObject();
        }

        void WriteEntity(JsonWriter& writer, const Entity entity, const bool withComponents)
        {
            writer.BeginObject();
            writer.KeyValue("id", static_cast<uint64_t>(entity.GetUUID()));
            writer.KeyValue("tag", entity.GetName());

            const auto& transform = entity.GetComponent<Transform>();

            if (const auto parent = transform.GetParent())
                writer.KeyValue("parent", static_cast<uint64_t>(parent.GetUUID()));
            else
                writer.Key("parent").Null();

            writer.KeyVector3("position", transform.GetWorldPosition());

            if (withComponents)
            {
                writer.Key("components").BeginArray();
                writer.Value(k_TransformKey);

                for (const auto& serializer : ClassCollection<IComponentSerializer>::Instance())
                {
                    if (serializer->HasComponent(entity))
                        writer.Value(String{ serializer->GetComponentKey() });
                }

                writer.EndArray();
            }

            writer.EndObject();
        }
    }

    Entity AgentBridge::FindEntity(const YAML::Node& node, String& outError)
    {
        const auto& scene = SceneManager::GetActiveScene();

        if (!scene)
        {
            outError = "no scene is loaded";
            return Entity{};
        }

        String identifier = JsonRead::GetString(node, "entity", String{});

        if (identifier.empty())
            identifier = JsonRead::GetString(node, "id", String{});

        if (identifier.empty())
            identifier = JsonRead::GetString(node, "uuid", String{});

        const auto tag = JsonRead::GetString(node, "tag", String{});

        if (identifier.empty() && tag.empty())
        {
            outError = "entity is required, give a UUID or a tag";
            return Entity{};
        }

        if (!identifier.empty())
        {
            char* end = nullptr;
            const auto asNumber = std::strtoull(identifier.c_str(), &end, 10);

            if (end && *end == '\0')
            {
                if (const Entity entity = scene->GetEntityByUUID(UUID(asNumber)))
                    return entity;

                outError = Format("no entity with id {0}", identifier);
                return Entity{};
            }
        }

        //<--- A name, then. The first match wins, which is why the answer carries the id back ---<<
        const String wanted = identifier.empty() ? tag : identifier;

        Entity found;
        scene->ForEachEntity([&](const Entity entity)
        {
            if (found || entity.GetName() != wanted) return;

            found = entity;
        });

        if (!found)
            outError = Format("no entity called '{0}'", wanted);

        return found;
    }

    String AgentBridge::CommandEntityList(const YAML::Node& arguments, const Ref<AgentServer::Call>&)
    {
        const auto& scene = SceneManager::GetActiveScene();
        if (!scene) return Error("no scene is loaded");

        const auto tagFilter = JsonRead::GetString(arguments, "tag", String{});
        const auto componentFilter = JsonRead::GetString(arguments, "component", String{});
        const int limit = JsonRead::GetInt(arguments, "limit", 500);
        const bool withComponents = JsonRead::GetBool(arguments, "components", true);

        Ref<IComponentSerializer> filterSerializer;

        if (!componentFilter.empty() && componentFilter != k_TransformKey)
        {
            filterSerializer = FindSerializer(componentFilter);

            if (!filterSerializer)
                return Error(Format("no component called '{0}', call component_list", componentFilter));
        }

        JsonWriter writer;
        writer.BeginObject();
        writer.KeyValue("ok", true);
        writer.Key("entities").BeginArray();

        int written = 0;
        int matched = 0;

        scene->ForEachEntity([&](const Entity entity)
        {
            if (!entity) return;

            //<--- The scene's root is an implementation detail, not something to act on ---<<
            if (entity == scene->GetRootEntity()) return;

            if (!tagFilter.empty() && entity.GetName().find(tagFilter) == String::npos) return;

            if (filterSerializer && !filterSerializer->HasComponent(entity)) return;

            matched++;

            if (written >= limit) return;

            WriteEntity(writer, entity, withComponents);
            written++;
        });

        writer.EndArray();
        writer.KeyValue("count", matched);
        writer.KeyValue("returned", written);

        if (matched > written)
            writer.KeyValue("note", Format("{0} entities matched, {1} returned, raise 'limit' for more", matched, written));

        writer.EndObject();

        return writer.Str();
    }

    String AgentBridge::CommandEntityCreate(const YAML::Node& arguments, const Ref<AgentServer::Call>&)
    {
        const auto& scene = SceneManager::GetActiveScene();
        if (!scene) return Error("no scene is loaded");

        const auto tag = JsonRead::GetString(arguments, "name", JsonRead::GetString(arguments, "tag", "Entity"));

        Entity entity;

        if (JsonRead::Has(arguments, "parent"))
        {
            YAML::Node parentNode{ YAML::NodeType::Map };
            parentNode["entity"] = arguments["parent"];

            String error;
            const Entity parent = FindEntity(parentNode, error);

            if (!parent) return Error(Format("parent: {0}", error));

            entity = scene->CreateEntity(tag, parent);
        }
        else
        {
            entity = scene->CreateEntity(tag);
        }

        if (!entity) return Error("the scene refused to create the entity");

        auto& transform = entity.GetComponent<Transform>();

        if (Vector3 position; JsonRead::TryGetVector3(arguments, "position", position))
            transform.SetWorldPosition(position);

        if (Vector3 rotation; JsonRead::TryGetVector3(arguments, "rotation", rotation))
            transform.SetWorldRotation(Quaternion::CreateFromYawPitchRoll(rotation * Math::DegToRad));

        if (Vector3 scale; JsonRead::TryGetVector3(arguments, "scale", scale))
            transform.SetLocalScale(scale);

        Vector<String> added;
        Vector<String> unknown;

        if (const auto& components = arguments["components"]; components && components.IsSequence())
        {
            for (const auto& component : components)
            {
                const String key{ component.Scalar().c_str() };

                if (const auto serializer = FindSerializer(key))
                {
                    serializer->AddComponent(entity);
                    added.push_back(key);
                }
                else
                {
                    unknown.push_back(key);
                }
            }
        }

        //<--- What the agent does to the scene is as undoable as what the buttons do ---<<
        RecordEntityCreated(entity, Format("Create {0}", entity.GetName()));

        JsonWriter writer;
        writer.BeginObject();
        writer.KeyValue("ok", true);
        writer.KeyValue("id", static_cast<uint64_t>(entity.GetUUID()));
        writer.KeyValue("tag", entity.GetName());

        writer.Key("added").BeginArray();
        for (const auto& key : added)
            writer.Value(key);
        writer.EndArray();

        if (!unknown.empty())
        {
            writer.Key("unknownComponents").BeginArray();
            for (const auto& key : unknown)
                writer.Value(key);
            writer.EndArray();
        }

        writer.EndObject();

        return writer.Str();
    }

    String AgentBridge::CommandEntityDestroy(const YAML::Node& arguments, const Ref<AgentServer::Call>&)
    {
        String error;
        const Entity entity = FindEntity(arguments, error);

        if (!entity) return Error(error);

        const auto id = static_cast<uint64_t>(entity.GetUUID());

        RecordEntityDestroyed(entity, Format("Delete {0}", entity.GetName()));

        //<--- Marks it, the scene destroys it at a point where nothing is iterating over it ---<<
        Scene::DestroyEntity(entity);

        return Ok(Format("\"id\":{0}", id));
    }

    String AgentBridge::CommandUndo(const YAML::Node& arguments, const Ref<AgentServer::Call>&)
    {
        const int requested = JsonRead::GetInt(arguments, "steps", 1);
        const int steps = requested < 1 ? 1 : requested;

        JsonWriter writer;
        writer.BeginObject();
        writer.KeyValue("ok", true);
        writer.Key("undone").BeginArray();

        for (int i = 0; i < steps && UndoSystem::CanUndo(); i++)
        {
            writer.Value(UndoSystem::GetUndoName());
            UndoSystem::Undo();
        }

        writer.EndArray();
        writer.KeyValue("canUndo", UndoSystem::CanUndo());
        writer.KeyValue("canRedo", UndoSystem::CanRedo());
        writer.EndObject();

        return writer.Str();
    }

    String AgentBridge::CommandRedo(const YAML::Node& arguments, const Ref<AgentServer::Call>&)
    {
        const int requested = JsonRead::GetInt(arguments, "steps", 1);
        const int steps = requested < 1 ? 1 : requested;

        JsonWriter writer;
        writer.BeginObject();
        writer.KeyValue("ok", true);
        writer.Key("redone").BeginArray();

        for (int i = 0; i < steps && UndoSystem::CanRedo(); i++)
        {
            writer.Value(UndoSystem::GetRedoName());
            UndoSystem::Redo();
        }

        writer.EndArray();
        writer.KeyValue("canUndo", UndoSystem::CanUndo());
        writer.KeyValue("canRedo", UndoSystem::CanRedo());
        writer.EndObject();

        return writer.Str();
    }

    String AgentBridge::CommandComponentList(const YAML::Node&, const Ref<AgentServer::Call>&)
    {
        JsonWriter writer;
        writer.BeginObject();
        writer.KeyValue("ok", true);
        writer.Key("components").BeginArray();

        writer.Value(k_TransformKey);

        for (const auto& serializer : ClassCollection<IComponentSerializer>::Instance())
            writer.Value(String{ serializer->GetComponentKey() });

        writer.EndArray();
        writer.EndObject();

        return writer.Str();
    }

    String AgentBridge::CommandComponentGet(const YAML::Node& arguments, const Ref<AgentServer::Call>&)
    {
        String error;
        const Entity entity = FindEntity(arguments, error);

        if (!entity) return Error(error);

        const auto key = JsonRead::GetString(arguments, "component", String{});
        if (key.empty()) return Error("component is required, call component_list for the keys");

        JsonWriter writer;
        writer.BeginObject();
        writer.KeyValue("ok", true);
        writer.KeyValue("id", static_cast<uint64_t>(entity.GetUUID()));
        writer.KeyValue("component", key);

        if (key == k_TransformKey)
        {
            writer.Key("values");
            WriteTransform(writer, entity.GetComponent<Transform>());
            writer.EndObject();

            return writer.Str();
        }

        const auto serializer = FindSerializer(key);
        if (!serializer) return Error(Format("no component called '{0}', call component_list", key));

        if (!serializer->HasComponent(entity))
            return Error(Format("that entity has no {0}", key));

        const YAML::Node serialized = SerializeEntity(entity);
        const YAML::Node values = serialized[key.c_str()];

        writer.Key("values");

        if (values)
            WriteYamlAsJson(writer, values);
        else
            writer.BeginObject().EndObject();

        writer.EndObject();

        return writer.Str();
    }

    String AgentBridge::CommandComponentSet(const YAML::Node& arguments, const Ref<AgentServer::Call>& call)
    {
        String error;
        const Entity entity = FindEntity(arguments, error);

        if (!entity) return Error(error);

        const auto key = JsonRead::GetString(arguments, "component", String{});
        if (key.empty()) return Error("component is required, call component_list for the keys");

        const YAML::Node values = CollectValues(arguments);

        if (!values || !values.IsMap() || values.size() == 0)
            return Error("nothing to set, pass the fields in 'values'");

        ScopedEntityEdit edit{ entity, Format("Set {0} of {1}", key, entity.GetName()) };

        if (key == k_TransformKey)
        {
            auto& transform = entity.GetComponent<Transform>();

            if (Vector3 position; JsonRead::TryGetVector3(values, "position", position))
                transform.SetLocalPosition(position);

            if (Vector3 worldPosition; JsonRead::TryGetVector3(values, "worldPosition", worldPosition))
                transform.SetWorldPosition(worldPosition);

            if (Vector3 rotation; JsonRead::TryGetVector3(values, "rotation", rotation))
                transform.SetLocalRotation(Quaternion::CreateFromYawPitchRoll(rotation * Math::DegToRad));

            if (Vector3 worldRotation; JsonRead::TryGetVector3(values, "worldRotation", worldRotation))
                transform.SetWorldRotation(Quaternion::CreateFromYawPitchRoll(worldRotation * Math::DegToRad));

            if (Vector3 scale; JsonRead::TryGetVector3(values, "scale", scale))
                transform.SetLocalScale(scale);

            //<--- Recorded before the answer is written, so the answer describes what was recorded ---<<
            edit.Commit();

            return CommandComponentGet(arguments, call);
        }

        const auto serializer = FindSerializer(key);
        if (!serializer) return Error(Format("no component called '{0}', call component_list", key));

        if (!serializer->HasComponent(entity))
            return Error(Format("that entity has no {0}, add it with component_add", key));

        const YAML::Node serialized = SerializeEntity(entity);
        const YAML::Node merged = MergeNodes(serialized[key.c_str()], values);

        // Written back through the component's own deserializer, so a value goes through the same
        // code a scene file does -- including whatever the component does with it afterwards, like a
        // planet rebuilding what the change invalidated.
        YAML::Node wrapper{ YAML::NodeType::Map };
        wrapper[key.c_str()] = merged;

        try
        {
            serializer->Deserialize(wrapper, entity);
        }
        catch (const std::exception& exception)
        {
            return Error(Format("{0} rejected the values: {1}", key, exception.what()));
        }

        edit.Commit();

        return CommandComponentGet(arguments, call);
    }

    String AgentBridge::CommandComponentAdd(const YAML::Node& arguments, const Ref<AgentServer::Call>& call)
    {
        String error;
        const Entity entity = FindEntity(arguments, error);

        if (!entity) return Error(error);

        const auto key = JsonRead::GetString(arguments, "component", String{});
        if (key.empty()) return Error("component is required, call component_list for the keys");

        if (key == k_TransformKey)
            return Error("every entity already has a Transform");

        const auto serializer = FindSerializer(key);
        if (!serializer) return Error(Format("no component called '{0}', call component_list", key));

        bool added;
        {
            //<--- Committed before the values are set, so adding and setting are two steps that
            //undo in the order they were made ---<<
            ScopedEntityEdit edit{ entity, Format("Add {0} to {1}", key, entity.GetName()) };
            added = serializer->AddComponent(entity);
        }

        //<--- Adding and setting in one call, because a default is rarely what is wanted ---<<
        if (const YAML::Node values = CollectValues(arguments); values && values.IsMap() && values.size() > 0)
            return CommandComponentSet(arguments, call);

        return Ok(Format("\"component\":\"{0}\",\"added\":{1}", key, added ? "true" : "false"));
    }

    String AgentBridge::CommandComponentRemove(const YAML::Node& arguments, const Ref<AgentServer::Call>&)
    {
        String error;
        const Entity entity = FindEntity(arguments, error);

        if (!entity) return Error(error);

        const auto key = JsonRead::GetString(arguments, "component", String{});
        if (key.empty()) return Error("component is required");

        if (key == k_TransformKey)
            return Error("a Transform cannot be removed, every entity has one");

        const auto serializer = FindSerializer(key);
        if (!serializer) return Error(Format("no component called '{0}', call component_list", key));

        bool removed;
        {
            ScopedEntityEdit edit{ entity, Format("Remove {0} from {1}", key, entity.GetName()) };
            removed = serializer->RemoveComponent(entity);
        }

        return Ok(Format("\"component\":\"{0}\",\"removed\":{1}", key, removed ? "true" : "false"));
    }
}
