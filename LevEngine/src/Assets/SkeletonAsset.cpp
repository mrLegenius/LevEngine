#include "levpch.h"
#include "SkeletonAsset.h"

#include "Renderer/3D/Skeleton.h"

namespace LevEngine
{
    SkeletonAsset::SkeletonAsset(const Path& path, const UUID uuid) : Asset(path, uuid) { }

    const Ref<Skeleton>& SkeletonAsset::GetSkeleton() const
    {
        return m_Skeleton;
    }

    void SkeletonAsset::SetSkeleton(const Ref<Skeleton>& skeleton)
    {
        m_Skeleton = skeleton;
    }

    void SkeletonAsset::SerializeData(YAML::Emitter& out)
    {
        
    }

    void SkeletonAsset::DeserializeData(const YAML::Node& node)
    {
        
    }

    void SkeletonAsset::SerializeToBinaryLibrary()
    {
        if (m_Skeleton == nullptr) return;
        
        m_Skeleton->Serialize(m_LibraryPath);
    }

    void SkeletonAsset::DeserializeFromBinaryLibrary()
    {
        if (m_Skeleton != nullptr) return;
        
        m_Skeleton = CreateRef<Skeleton>();
        m_Skeleton->Deserialize(m_LibraryPath);
    }
}

