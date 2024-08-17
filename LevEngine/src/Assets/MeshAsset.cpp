#include "levpch.h"
#include "MeshAsset.h"

#include "AnimationAsset.h"
#include "AssetDatabase.h"
#include "EngineAssets.h"
#include "Cache/MeshAssetCache.h"
#include "Renderer/3D/MeshLoading/AnimationLoader.h"
#include "Renderer/3D/MeshLoading/ModelParser.h"

namespace LevEngine
{
    MeshAsset::MeshAsset(const Path& path, const UUID uuid, const Ref<Mesh>& mesh)
        : Asset(path, uuid), m_Mesh(mesh)
    {
        MeshAssetCache::SaveToCache(uuid, mesh);
    }

    Ref<Texture> MeshAsset::GetIcon() const
    {
        return Icons::Mesh();
    }

    void MeshAsset::DeserializeData(const YAML::Node& node)
    {
        LEV_PROFILE_FUNCTION();

        if (m_Mesh) return;
        
        try
        {
            auto cachedMesh = MeshAssetCache::LoadFromCache(m_UUID);
            
            m_Mesh = cachedMesh ? cachedMesh : ModelParser::LoadModel(m_Path);

            if (!cachedMesh)
                MeshAssetCache::SaveToCache(m_UUID, m_Mesh);
        }
        catch (std::exception& e)
        {
            Log::CoreWarning("Failed to load mesh in {0}. Error: {1}", m_Path.string(), e.what());
        }

        try
        {
            LoadAnimations();
        }
        catch (std::exception& e)
        {
            Log::CoreWarning("Failed to load animations in {0}. Error: {1}", m_Path.string(), e.what());
        }
    }

    void MeshAsset::LoadAnimations()
    {
        LEV_PROFILE_FUNCTION();
        const auto animations = AnimationLoader::LoadAllAnimations(m_Path, m_Mesh);
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
                
            if (!AssetDatabase::AssetExists(animationAssetPath))
            {
                const Ref<AnimationAsset> animationAsset =
                    AssetDatabase::CreateNewAsset<AnimationAsset>(animationAssetPath);

                animationAsset->Init(animations[animationIdx], animationIdx, shared_from_this());
                animationAsset->Serialize();
            }
        }
    }
}
