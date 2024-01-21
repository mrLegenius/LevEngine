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
        explicit SkeletonAsset(const Path& path, const UUID uuid);

        [[nodiscard]] const Ref<Skeleton>& GetSkeleton() const;
        void Init(const Ref<Skeleton>& skeleton);

    protected:
        void SerializeData(YAML::Emitter& out) override;
        void DeserializeData(const YAML::Node& node) override;

    private:
        const char* c_OwnerMeshKey = "OwnerMesh";
		
        Ref<Skeleton> m_Skeleton;
    };
}
