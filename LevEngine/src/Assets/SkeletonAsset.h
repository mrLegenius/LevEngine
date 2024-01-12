#pragma once
#include "Asset.h"

namespace LevEngine
{
    class Skeleton;
    class MeshAsset;
    class Animation;

    class SkeletonAsset final : public Asset
    {
    public:
        explicit SkeletonAsset(const Path& path, const UUID uuid) : Asset(path, uuid)
        {
            m_AnimationIdx = -1;
        }

        [[nodiscard]] const Ref<Skeleton>& GetSkeleton() const;
        
        [[nodiscard]] const Ref<MeshAsset>& GetOwnerMesh() const;
        void SetOwnerMesh(const Ref<MeshAsset>& ownerMesh);

    protected:
        void SerializeData(YAML::Emitter& out) override;

        void DeserializeData(YAML::Node& node) override;

    private:
        const char* c_AnimationIndexKey = "AnimationIndexInModelFile";
        const char* c_OwnerMeshKey = "OwnerMesh";
		
        Ref<Skeleton> m_Skeleton;
        int m_AnimationIdx;
        Ref<MeshAsset> m_OwnerMesh;
    };
}
