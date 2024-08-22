#pragma once
#include "Asset.h"
#include "AI/Components/AIAgentCrowdComponent.h"
#include "AI/Components/AIAgentCrowdComponent.h"
#include "Renderer/3D/Animation.h"
#include "Scene/Scene.h"
#include "Renderer/3D/BoneInfo.h"

namespace LevEngine
{
    struct ModelNode;
    class Mesh;
    
    class ModelAsset final : public Asset, public eastl::enable_shared_from_this<ModelAsset>
    {
    public:
        ModelAsset(const Path& path, const UUID& uuid) : Asset(path, uuid) { }
        ~ModelAsset() override = default;

        Ref<Texture> GetIcon() const override;
        Entity InstantiateModel(const Ref<Scene>& scene) const;

        float Scale = 1;

        void Clear() override;

        [[nodiscard]] int& GetBoneCount() { return m_BoneCounter; }
        void SetBoneCount(int value) { m_BoneCounter = value; }
        [[nodiscard]] UnorderedMap<String, BoneInfo>& GetBoneInfoMap() { return m_BoneInfoMap; }
    
    protected:
        bool LoadFromCache() override;
        void SaveToCache() override;
        
        void SerializeData(YAML::Emitter& out) override { }
        void DeserializeData(const YAML::Node& node) override;

        void SerializeMeta(YAML::Emitter& out) override;
        void DeserializeMeta(const YAML::Node& node) override;

        bool ReadDataFromFile() const override { return false; }
        bool WriteDataToFile() const override { return false; }
        
    private:
        
        void LoadAnimations(Vector<Ref<Animation>> animations);
        UnorderedMap<String, BoneInfo> m_BoneInfoMap;
        int m_BoneCounter = 0;
        
        ModelNode* m_Hierarchy{};
        
        Vector<Ref<Mesh>> m_Meshes;
    };
}
