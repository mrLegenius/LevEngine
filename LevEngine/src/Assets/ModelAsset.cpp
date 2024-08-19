#include "levpch.h"
#include "ModelAsset.h"

#include "AssetDatabase.h"
#include "EngineAssets.h"
#include "MaterialPBRAsset.h"
#include "MeshAsset.h"
#include "ModelNode.h"
#include "Cache/ModelAssetCache.h"
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

        if (node->MeshUUID || node->MaterialUUID)
        {
            auto& meshRenderer = entity.GetOrAddComponent<MeshRendererComponent>();
            
            if (node->MeshUUID)
                meshRenderer.mesh = AssetDatabase::GetAsset<MeshAsset>(node->MeshUUID);

            if (node->MaterialUUID)
                meshRenderer.material = AssetDatabase::GetAsset<MaterialPBRAsset>(node->MaterialUUID);
            
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

    void ModelAsset::Clear()
    {
        if (!m_Hierarchy) return;

        Vector<ModelNode*> nodes;
        nodes.push_back(m_Hierarchy);

        while (nodes.size())
        {
            auto elem = nodes[0];
            nodes.erase(nodes.begin());

            if (auto mesh = elem->MeshUUID ? AssetDatabase::GetAsset<MeshAsset>(elem->MeshUUID) : nullptr)
                AssetDatabase::DeleteAsset(mesh);
            
            if (auto material = elem->MaterialUUID ? AssetDatabase::GetAsset<MaterialPBRAsset>(elem->MaterialUUID) : nullptr)
                AssetDatabase::DeleteAsset(material);

            for (auto child : elem->Children)
                nodes.push_back(child);
        }
    }

    bool ModelAsset::LoadFromCache()
    {
        m_Hierarchy = ModelAssetCache::LoadFromCache(m_UUID);
        return m_Hierarchy;
    }

    void ModelAsset::SaveToCache()
    {
        if (m_Hierarchy)
            ModelAssetCache::SaveToCache(m_UUID, m_Hierarchy);
    }
    
    void ModelAsset::DeserializeData(const YAML::Node& node)
    {
        ModelImportParameters params{};
        params.scale = Scale;
        
        m_Hierarchy = ModelParser::Load(m_Path, params).Hierarchy;
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
