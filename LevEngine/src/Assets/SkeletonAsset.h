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
        void SetSkeleton(const Ref<Skeleton>& skeleton);

    protected:
        [[nodiscard]] bool WriteLibraryToFile() const override { return true; }
        [[nodiscard]] bool ReadLibraryFromFile() const override { return true; }
        
        void SerializeData(YAML::Emitter& out) override;
        void DeserializeData(const YAML::Node& node) override;

        void SerializeToBinaryLibrary() override;
        void DeserializeFromBinaryLibrary() override;

    private:
        Ref<Skeleton> m_Skeleton;
    };
}
