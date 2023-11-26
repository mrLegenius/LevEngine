#include "levpch.h"
#include "MeshAsset.h"

#include "MeshLoader.h"

namespace LevEngine
{
    Ref<Texture> MeshAsset::GetIcon() const
    {
        return Icons::Mesh();
    }

    void MeshAsset::DeserializeData(YAML::Node& node)
    {
        try
        {
            m_Mesh = MeshLoader::LoadMesh(m_Path);
        }
        catch (std::exception& e)
        {
            Log::CoreWarning("Failed to load mesh in {0}. Error: {1}", m_Path.string(), e.what());
        }
    }
}
