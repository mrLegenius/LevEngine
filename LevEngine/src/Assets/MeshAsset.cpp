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
                String animationName = animations[animationIdx]->GetName();
                
                String nameToWrite = (animationName.empty()
                    ? ToString(animationIdx)
                      : animationName)
                + String(".anim");
                
                const Path animationAssetPath = m_Path.parent_path().append(nameToWrite.c_str());
                
                if (!AssetDatabase::AssetExists(animationAssetPath))
                {
                    const Ref<AnimationAsset> animationAsset =
                        AssetDatabase::CreateAsset<AnimationAsset>(animationAssetPath);
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
