#pragma once

#include "Asset.h"
#include "Scene/Scene.h"

namespace LevEngine
{
class PrefabAsset final : public Asset
{
public:
    PrefabAsset(const Path& path, const UUID& uuid) : Asset(path, uuid) { }

    Entity Instantiate(const Ref<Scene>& scene);
    void SaveEntity(Entity entity);

protected:
    [[nodiscard]] bool WriteDataToFile() const override { return m_EntityToSave; }
    
    void SerializeData(YAML::Emitter& out) override;
    void DeserializeData(YAML::Node& node) override
    {
        if (!node["Entities"])
            throw std::exception("File does not contain any Entity");

        m_DeserializedData = node;
    }
private:
    YAML::Node m_DeserializedData;
    Entity m_EntityToSave;
};
}
