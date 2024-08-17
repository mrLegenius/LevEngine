#include "levpch.h"
#include "MeshAssetCache.h"

#include "Assets/AssetDatabase.h"
#include "BinaryIO/BinaryReader.h"
#include "BinaryIO/BinaryWriter.h"
#include "Renderer/3D/Mesh.h"

//TODO: add versioning

namespace LevEngine
{
    Ref<Mesh> LoadMesh(BinaryReader& reader)
    {
        Ref<Mesh> mesh = CreateRef<Mesh>();

        reader.ReadForEach<uint32_t>([&mesh](auto data) { mesh->AddIndex(data); });
        reader.ReadForEach<Vector3>([&mesh](auto data) { mesh->AddVertex(data); });
        reader.ReadForEach<Vector3>([&mesh](auto data) { mesh->AddNormal(data); });
        reader.ReadForEach<Vector3>([&mesh](auto data) { mesh->AddTangent(data); });
        reader.ReadForEach<Vector2>([&mesh](auto data) { mesh->AddUV(data); });

        auto bonesCount = reader.Read<size_t>();
        Vector<Array<int, AnimationConstants::MaxBoneInfluence>> bones;
        bones.reserve(bonesCount);
        for (int i = 0; i < bonesCount; ++i)
            bones.push_back(reader.ReadArray<int, AnimationConstants::MaxBoneInfluence>());
        mesh->SetBoneIds(bones);

        auto weightsCount = reader.Read<size_t>();
        Vector<Array<float, AnimationConstants::MaxBoneInfluence>> weights;
        weights.reserve(weightsCount);
        for (int i = 0; i < weightsCount; ++i)
            weights.push_back(reader.ReadArray<float, AnimationConstants::MaxBoneInfluence>());
        mesh->SetWeights(weights);

        auto weightsCounters = reader.ReadVector<int>();
        mesh->SetBoneWeightCounters(weightsCounters);

        mesh->Init();
        mesh->GenerateAABBBoundingVolume();
        
        auto subMeshesCount = reader.Read<size_t>();
        for (int i = 0; i < subMeshesCount; ++i)
            mesh->AddSubMesh(LoadMesh(reader));

        return mesh;
    }
    
    Ref<Mesh> MeshAssetCache::LoadFromCache(UUID assetUUID)
    {
        LEV_PROFILE_FUNCTION();

        auto cachePath = AssetDatabase::GetAssetCachePath(assetUUID);

        if (!exists(cachePath)) return nullptr;

        try
        {
            BinaryReader reader{cachePath};

            return LoadMesh(reader);
        }
        catch (std::exception& e)
        {
            Log::CoreError("Failed to load mesh from cache. Error {0}", e.what());
        }

        return nullptr;
    }

    void SaveMesh(const Ref<Mesh>& mesh, BinaryWriter& writer)
    {
        writer.WriteVector(mesh->GetIndices());
        writer.WriteVector(mesh->GetVertices());
        writer.WriteVector(mesh->GetNormals());
        writer.WriteVector(mesh->GetTangents());
        writer.WriteVector(mesh->GetUVs());

        auto bones = mesh->GetBoneIds();
        writer.Write(bones.size());
        for (auto id : bones)
            writer.WriteArray(id);

        auto weights = mesh->GetWeights();
        writer.Write(weights.size());
        for (auto id : weights)
            writer.WriteArray(id);

        auto weightsCounters = mesh->GetBoneWeightCounters();
        writer.WriteVector(weightsCounters);

        auto subMeshes = mesh->GetSubMeshes();
        writer.Write(subMeshes.size());
        for (auto subMesh : subMeshes)
            SaveMesh(subMesh, writer);
    }

    void MeshAssetCache::SaveToCache(UUID assetUUID, const Ref<Mesh>& mesh)
    {
        LEV_PROFILE_FUNCTION();

        try
        {
            auto cachePath = AssetDatabase::GetAssetCachePath(assetUUID);

            BinaryWriter writer{cachePath};

            SaveMesh(mesh, writer);
        }
        catch (std::exception& e)
        {
            Log::CoreError("Failed to save mesh to cache. Error {0}", e.what());
        }
    }
}
