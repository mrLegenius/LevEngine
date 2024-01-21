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
        // if (!exists(m_MetaPath))
        // {
        //     ParseMeshFromUniversalFormat();
        //     return;
        // }
        //
        // auto metaNode = YAML::LoadFile(m_MetaPath.string());
        //
        // auto vertexNodes = metaNode[c_VertexListKey];
        // auto indexNodes = metaNode[c_IndexListKey];
        // auto uvNodes = metaNode[c_UVListKey];
        // auto normalNodes = metaNode[c_NormalListKey];
        // auto tangentNodes = metaNode[c_TangentListKey];
        // auto biTangentNodes = metaNode[c_BiTangentListKey];
        // auto boneIDNodes = metaNode[c_BoneIDListKey];
        // auto boneWeightNodes = metaNode[c_BoneWeightListKey];
        //
        // if (!vertexNodes || !indexNodes || !uvNodes || !normalNodes || !tangentNodes || !biTangentNodes)
        // {
        //     ParseMeshFromUniversalFormat();
        // }
    }

    void MeshAsset::SerializeToBinaryLibrary()
    {
        if (m_Mesh == nullptr)
        {
            ParseMeshFromUniversalFormat();
        }
        
        // SerializeList<Vector3>(out, c_VertexListKey, m_Mesh->GetVertices());
        // SerializeList<uint32_t>(out, c_IndexListKey, m_Mesh->GetIndices());
        // SerializeList<Vector2>(out, c_UVListKey, m_Mesh->GetUVs());
        // SerializeList<Vector3>(out, c_NormalListKey, m_Mesh->GetNormals());
        // SerializeList<Vector3>(out, c_TangentListKey, m_Mesh->GetTangents());
        // SerializeList<Vector3>(out, c_BiTangentListKey, m_Mesh->GetBiTangents());
        // SerializeList<Array<int, AnimationConstants::MaxBoneInfluence>>(out, c_BoneIDListKey, m_Mesh->GetBoneIds());
        // SerializeList<Array<float, AnimationConstants::MaxBoneInfluence>>(out, c_BoneWeightListKey, m_Mesh->GetBoneWeights());
        //
        // const auto aabb = m_Mesh->GetAABBBoundingVolume();
        // out << YAML::Key << c_AABBCenterKey << YAML::Value << aabb.center;
        // out << YAML::Key << c_AABBExtentsKey << YAML::Value << aabb.extents;

        m_Mesh->Serialize(m_LibraryPath);
    }

    void MeshAsset::DeserializeFromBinaryLibrary()
    {
        if (m_Mesh != nullptr)
        {
            return;
        }
        
        //m_Mesh = CreateRef<Mesh>();
        //
        // auto vertexNodes = node[c_VertexListKey];
        // auto indexNodes = node[c_IndexListKey];
        // auto uvNodes = node[c_UVListKey];
        // auto normalNodes = node[c_NormalListKey];
        // auto tangentNodes = node[c_TangentListKey];
        // auto biTangentNodes = node[c_BiTangentListKey];
        // auto boneIDArrayNodes = node[c_BoneIDListKey];
        // auto boneWeightArrayNodes = node[c_BoneWeightListKey];
        //
        // if (!vertexNodes || !indexNodes || !uvNodes || !normalNodes || !tangentNodes || !biTangentNodes)
        // {
        //     ParseMeshFromUniversalFormat();
        //     return;
        // }
        //
        // // Parse mesh from our YAML format
        // for (auto vertexNode : vertexNodes)
        // {
        //     m_Mesh->AddVertex(vertexNode.as<Vector3>());
        // }
        //
        // for (auto indexNode : indexNodes)
        // {
        //     m_Mesh->AddIndex(indexNode.as<uint32_t>());
        // }
        //
        // for (auto uvNode : uvNodes)
        // {
        //     m_Mesh->AddUV(uvNode.as<Vector2>());
        // }
        //
        // for (auto normalNode : normalNodes)
        // {
        //     m_Mesh->AddNormal(normalNode.as<Vector3>());
        // }
        //
        // for (auto tangentNode : tangentNodes)
        // {
        //     m_Mesh->AddTangent(tangentNode.as<Vector3>());
        // }
        //
        // for (auto biTangentNode : biTangentNodes)
        // {
        //     m_Mesh->AddBiTangent(biTangentNode.as<Vector3>());
        // }
        //
        // m_Mesh->ResizeBoneArrays(m_Mesh->GetVerticesCount());
        //
        // int vertexIdx = 0;
        // for (auto boneIDArrayNode : boneIDArrayNodes)
        // {
        //     auto boneWeightArrayNode = boneWeightArrayNodes[vertexIdx];
        //
        //     int weightIdx = 0;
        //     for (auto boneIDNode : boneIDArrayNode)
        //     {
        //         auto boneWeightNode = boneWeightArrayNode[weightIdx];
        //         m_Mesh->AddBoneWeight(vertexIdx, boneIDNode.as<int>(), boneWeightNode.as<float>());
        //         
        //         weightIdx++;
        //     }
        //
        //     ++vertexIdx;
        // }
        //
        // Vector3 aabbCenter = node[c_AABBCenterKey].as<Vector3>();
        // Vector3 aabbExtents = node[c_AABBExtentsKey].as<Vector3>();
        // m_Mesh->SetAABBBoundingVolume(aabbCenter, aabbExtents.x, aabbExtents.y, aabbExtents.z);
        //
        // m_Mesh->Init();

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

        if (resultSkeleton == nullptr)
        {
            return;
        }

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
    }
}
