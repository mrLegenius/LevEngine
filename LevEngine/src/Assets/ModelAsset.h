#pragma once
#include "Asset.h"
#include "AI/Components/AIAgentCrowdComponent.h"
#include "Scene/Scene.h"


namespace LevEngine
{
    struct ModelNode;
    class Mesh;
    
    class ModelAsset final : public Asset
    {
    public:
        ModelAsset(const Path& path, const UUID& uuid) : Asset(path, uuid) { }
        ~ModelAsset() override = default;

        Ref<Texture> GetIcon() const override;
        Entity InstantiateModel(const Ref<Scene>& scene) const;

        float Scale = 1;
        
    protected:
        void SerializeData(YAML::Emitter& out) override;
        void DeserializeData(const YAML::Node& node) override;

        void SerializeMeta(YAML::Emitter& out) override;
        void DeserializeMeta(const YAML::Node& node) override;

        bool WriteDataToFile() const override { return false; }
        
    private:
        
        ModelNode* m_Hierarchy{};
        
        Vector<Ref<Mesh>> m_Meshes;
    };
}
