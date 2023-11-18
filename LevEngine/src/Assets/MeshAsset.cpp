#include "levpch.h"
#include "MeshAsset.h"

#include "AnimationAsset.h"
#include "AssetDatabase.h"
#include "Renderer/3D/MeshLoading/AnimationLoader.h"
#include "Renderer/3D/MeshLoading/MeshLoader.h"

namespace LevEngine
{
    void MeshAsset::DeserializeData(YAML::Node& node)
    {
        try
        {
            m_Mesh = MeshLoader::LoadMesh(m_Path);
            const auto animations = AnimationLoader::LoadAllAnimations(m_Path, m_Mesh);
            for (int animationIdx = 0; animationIdx < animations.size(); ++animationIdx)
            {
                const Path animationAssetPath = m_Path.append(std::to_string(animationIdx));
                if (!AssetDatabase::AssetExists(animationAssetPath))
                {
                    const Ref<AnimationAsset> animationAsset =
                        AssetDatabase::CreateAsset<AnimationAsset>(animationAssetPath);
                    animationAsset->SetAnimation(animations[animationIdx]);
                    animationAsset->SetAnimationIdx(animationIdx);
                }
            }
        }
        catch (std::exception& e)
        {
            Log::CoreWarning("Failed to load animations in {0}. Error: {1}", m_Path.string(), e.what());
        }
    }
}
