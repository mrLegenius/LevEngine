#include "levpch.h"
#include "MeshAsset.h"

#include "AnimationAsset.h"
#include "AssetDatabase.h"
#include "EngineAssets.h"
#include "SkeletonAsset.h"
#include "Renderer/3D/MeshLoading/AnimationLoader.h"
#include "Renderer/3D/MeshLoading/MeshLoader.h"
#include "Scene/Serializers/SerializerUtils.h"

namespace LevEngine
{
    Ref<Texture> MeshAsset::GetIcon() const
    {
        return Icons::Mesh();
    }

    void MeshAsset::SerializeData(YAML::Emitter& out)
    {
        
    }

    void MeshAsset::DeserializeData(const YAML::Node& node)
    {

    }

    void MeshAsset::SerializeToBinaryLibrary()
    {
        if (m_Mesh == nullptr)
        {
            ParseMeshFromUniversalFormat();
        }
        
        m_Mesh->Serialize(m_LibraryPath);
    }

    void MeshAsset::DeserializeFromBinaryLibrary()
    {
        if (m_Mesh != nullptr)
        {
            return;
        }

        m_Mesh = CreateRef<Mesh>();
        m_Mesh->Deserialize(m_LibraryPath);
    }

    void MeshAsset::ParseMeshFromUniversalFormat()
    {
        Ref<Skeleton> resultSkeleton = nullptr;
        
        try
        {
            m_Mesh = MeshLoader::LoadMesh(m_Path, resultSkeleton);
        }
        catch (std::exception& e)
        {
            Log::CoreWarning("Failed to load mesh in {0}. Error: {1}", m_Path.string(), e.what());
        }

        if (resultSkeleton == nullptr) return;

        Vector<Ref<Animation>> animations;
        
        try
        {
            animations = AnimationLoader::LoadAllAnimations(m_Path, resultSkeleton);
        }
        catch (std::exception& e)
        {
            Log::CoreWarning("Failed to load animations in {0}. Error: {1}", m_Path.string(), e.what());
        }

        const Ref<SkeletonAsset> skeletonAsset = CreateSkeletonAsset(resultSkeleton);
        if (skeletonAsset == nullptr) return;
        
        CreateAnimationAsset(animations, skeletonAsset);
    }

    void MeshAsset::CreateAnimationAsset(const Vector<Ref<Animation>>& animations,
                                         const Ref<SkeletonAsset>& skeletonAsset) const
    {
        for (int animationIdx = 0; animationIdx < animations.size(); ++animationIdx)
        {
            String animationName = animations[animationIdx]->GetName();

            const size_t idx = animationName.find_last_of("|");
            if (idx != String::npos)
            {
                animationName = animationName.substr(idx + 1);
            }
                
            String nameToWrite = (animationName.empty()
                                      ? ToString(animationIdx)
                                      : animationName)
                + String(".anim");
                
            const Path animationAssetPath = m_Path.parent_path().append(nameToWrite.c_str());
                
            if (!std::filesystem::exists(animationAssetPath))
            {
                const Ref<AnimationAsset> animationAsset =
                    AssetDatabase::CreateNewAsset<AnimationAsset>(animationAssetPath);

                animationAsset->Init(animations[animationIdx], animationIdx, skeletonAsset);
                animationAsset->Serialize();
            }
        }
    }

    Ref<SkeletonAsset> MeshAsset::CreateSkeletonAsset(const Ref<Skeleton>& resultSkeleton)
    {
        const Path skeletonAssetPath = m_Path.parent_path().append((String(m_Name) + String(".skeleton")).c_str());
        if (!std::filesystem::exists(skeletonAssetPath))
        {
            const Ref<SkeletonAsset> skeletonAsset =
                AssetDatabase::CreateNewAsset<SkeletonAsset>(skeletonAssetPath);

            skeletonAsset->Init(resultSkeleton);
            skeletonAsset->Serialize();

            return skeletonAsset;
        }

        return nullptr;
    }
}
