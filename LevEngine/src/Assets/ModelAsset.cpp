#include "levpch.h"
#include "ModelAsset.h"

#include "AssetDatabase.h"
#include "EngineAssets.h"
#include "MeshAsset.h"
#include "ModelNode.h"
#include "Renderer/3D/MeshLoading/ModelParser.h"
#include "Scene/Components/MeshRenderer/MeshRenderer.h"
#include "Scene/Components/Transform/Transform.h"
#include "Scene/Serializers/SerializerUtils.h"

namespace LevEngine
{
    Ref<Texture> ModelAsset::GetIcon() const
    {
        return Icons::Model();
    }

    Entity InstantiateNode(const Ref<Scene>& scene, ModelNode* node)
    {
        auto entity = scene->CreateEntity(node->Name);
        
        auto& transform = entity.GetComponent<Transform>();
        
        Vector3 position;
        Quaternion rotation;
        Vector3 scale;
        
        Math::DecomposeTransform(node->Transform, position, rotation, scale);
        
        transform.SetLocalPosition(position);
        transform.SetLocalRotation(rotation);
        transform.SetLocalScale(scale);

        if (node->MeshUUID)
        {
            auto& meshRenderer = entity.GetOrAddComponent<MeshRendererComponent>();
            meshRenderer.mesh = AssetDatabase::GetAsset<MeshAsset>(node->MeshUUID);
            meshRenderer.enabled = true;
        }

        for (auto childNode : node->Children)
        {
            auto child = InstantiateNode(scene, childNode);
            auto& childTransform = child.GetComponent<Transform>();
            childTransform.SetParent(entity);
        }

        return entity;
    }

    Entity ModelAsset::InstantiateModel(const Ref<Scene>& scene) const
    {
        if (!m_Hierarchy) return Entity{};
        
        return InstantiateNode(scene, m_Hierarchy);
    }

    void ModelAsset::SerializeData(YAML::Emitter& out)
    {
    }

    void ModelAsset::DeserializeData(const YAML::Node& node)
    {
        ModelImportParameters params{};
        params.scale = Scale;
        auto result = ModelParser::Load(m_Path, params);
        m_Hierarchy = result.Hierarchy;
    }

    void ModelAsset::SerializeMeta(YAML::Emitter& out)
    {
        Write(out, "Scale", Scale);
    }

    void ModelAsset::DeserializeMeta(const YAML::Node& node)
    {
        TryParse(node["Scale"], Scale);
    }
}
