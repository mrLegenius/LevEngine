#include "levpch.h"
#include "MeshAsset.h"

#include "AnimationAsset.h"
#include "AssetDatabase.h"
#include "EngineAssets.h"
#include "Renderer/3D/MeshLoading/AnimationLoader.h"
#include "Renderer/3D/MeshLoading/MeshLoader.h"

namespace LevEngine
{
    Ref<Texture> MeshAsset::GetIcon() const
    {
        return Icons::Mesh();
    }

    void MeshAsset::DeserializeData(const YAML::Node& node)
    {
        try
        {
            Ref<Skeleton> resultSkeleton = nullptr;
            
            m_Mesh = MeshLoader::LoadMesh(m_Path, resultSkeleton);
            const auto animations = AnimationLoader::LoadAllAnimations(m_Path, resultSkeleton);


            Path skeletonAssetPath = m_Path.parent_path().append(".skeleton");
            if (!AssetDatabase::AssetExists(skeletonAssetPath))
            {
                const Ref<AnimationAsset> animationAsset =
                    eastl::static_shared_pointer_cast<AnimationAsset>(
                        AssetDatabase::CreateAsset(skeletonAssetPath));
                    
                animationAsset->SetAnimation(animations[animationIdx]);
                animationAsset->SetAnimationIdx(animationIdx);
                animationAsset->SetOwnerMesh(shared_from_this());
                animationAsset->Serialize();
            }


            for (int animationIdx = 0; animationIdx < animations.size(); ++animationIdx)
            {
                String animationName = animations[animationIdx]->GetName();

                const size_t idx = animationName.find_last_of("|");
                if (idx != String::npos)
                {
                    animationName = animationName.substr(idx + 1);
                }

                if (animationName.empty())
                {
                    animationName = ToString(animationIdx) + String(".anim");
                }
                else
                {
                    animationName += String(".anim");
                }
                
                const Path animationAssetPath = m_Path.parent_path().append(animationName.c_str());
                
                if (!AssetDatabase::AssetExists(animationAssetPath))
                {
                    const Ref<AnimationAsset> animationAsset =
                        eastl::static_shared_pointer_cast<AnimationAsset>(
                            AssetDatabase::CreateAsset(animationAssetPath));
                    
                    animationAsset->SetAnimation(animations[animationIdx]);
                    animationAsset->SetAnimationIdx(animationIdx);
                    animationAsset->SetOwnerMesh(shared_from_this());
                    animationAsset->Serialize();
                }
            }
        }
        catch (std::exception& e)
        {
            Log::CoreWarning("Failed to load animations in {0}. Error: {1}", m_Path.string(), e.what());
        }
    }
}
