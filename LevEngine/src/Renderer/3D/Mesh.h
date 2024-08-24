#pragma once

#include "AnimationConstants.h"
#include "Math/BoundingVolume.h"
#include "Renderer/Shader/BufferBinding.h"

namespace LevEngine
{
    class Shader;
    class VertexBuffer;
    class IndexBuffer;
    struct Color;
    
    class Mesh
    {
        struct MeshVertex
        {
            Vector3 Position;
            Vector3 Normal;
            Vector2 UV;
        };

    public:
        Mesh() = default;

        Ref<IndexBuffer> CreateIndexBuffer() const;

        void Clear();

        [[nodiscard]] uint32_t GetVerticesCount() const { return static_cast<uint32_t>(vertices.size()); }
        [[nodiscard]] const Vector<Vector3>& GetVertices() { return vertices; }
        [[nodiscard]] Vector3 GetVertex(const uint32_t index) const { return vertices[index]; }
        void AddVertex(const Vector3& value) { vertices.emplace_back(value); }

        [[nodiscard]] uint32_t GetIndicesCount() const { return static_cast<uint32_t>(indices.size()); }
        [[nodiscard]] const Vector<uint32_t>& GetIndices() const { return indices; }
        [[nodiscard]] uint32_t GetIndex(const uint32_t index) const { return indices[index]; }
        void AddTriangle(const Vector3& value);
        void AddIndex(const uint32_t& value) { indices.emplace_back(value); }

    	[[nodiscard]] size_t GetUVsCount() const { return uvs.size(); }
    	[[nodiscard]] const Vector<Vector2>& GetUVs() { return uvs; }
        [[nodiscard]] Vector2 GetUV(const uint32_t index) const { return uvs[index]; }
        void AddUV(const Vector2& value) { uvs.emplace_back(value); }

        [[nodiscard]] size_t GetNormalsCount() const { return normals.size(); }
    	[[nodiscard]] const Vector<Vector3>& GetNormals() { return normals; }
        [[nodiscard]] Vector3 GetNormal(const uint32_t index) const { return normals[index]; }
        void AddNormal(const Vector3& value) { normals.emplace_back(value); }

    	[[nodiscard]] size_t GetTangentsCount() const { return tangents.size(); }
    	[[nodiscard]] const Vector<Vector3>& GetTangents() { return tangents; }
    	[[nodiscard]] Vector3 GetTangent(const uint32_t index) const { return tangents[index]; }
        void AddTangent(Vector3 value) { tangents.emplace_back(value); }

        void AddVertexBuffer(const BufferBinding& binding, const Ref<VertexBuffer>& buffer);
        [[nodiscard]] const Map<BufferBinding, Ref<VertexBuffer>>& GetVertexBuffers() const { return m_VertexBuffers; }

        void Init();
        void GenerateAABBBoundingVolume();

        void Bind(const Ref<Shader>& shader) const;

        void SetAABBBoundingVolume(const Vector3& min, const Vector3& max) { m_BoundingVolume = {min, max}; }
        [[nodiscard]] const AABBBoundingVolume& GetAABBBoundingVolume() { return m_BoundingVolume; }

        [[nodiscard]] bool IsOnFrustum(const Frustum& frustum, const Transform& meshTransform) const;
		
		[[nodiscard]] Vector<Array<float, AnimationConstants::MaxBoneInfluence>>& GetBoneWeights();

		void AddBoneWeight(int vertexIdx, int boneID, float weight);
		void ResizeBoneArrays(size_t size);
		void NormalizeBoneWeights();

    	[[nodiscard]] const Vector<Array<int, AnimationConstants::MaxBoneInfluence>>& GetBoneIds() const { return m_BoneIds; }
    	void SetBoneIds(const Vector<Array<int, AnimationConstants::MaxBoneInfluence>>& data) { m_BoneIds = data; }
    	
    	[[nodiscard]] const Vector<Array<float, AnimationConstants::MaxBoneInfluence>>& GetWeights() const { return m_Weights; }
    	void SetWeights(const Vector<Array<float, AnimationConstants::MaxBoneInfluence>>& data) { m_Weights = data; }
    	
    	[[nodiscard]] const Vector<int>& GetBoneWeightCounters() const { return m_BoneWeightCounters; }
    	void SetBoneWeightCounters(const Vector<int>& data ) { m_BoneWeightCounters = data; }
    	
    	[[nodiscard]] const Vector<Ref<Mesh>>& GetSubMeshes() const { return m_SubMeshes; }
    	void AddSubMesh(const Ref<Mesh>& mesh) { return m_SubMeshes.push_back(mesh); }
    	
        Ref<IndexBuffer> IndexBuffer;

    private:
        Map<BufferBinding, Ref<VertexBuffer>> m_VertexBuffers;

    	// Mesh vertex data
        Vector<Vector3> vertices;
        Vector<Vector2> uvs;
        Vector<uint32_t> indices;
        Vector<Vector3> normals;
        Vector<Vector3> tangents;
        Vector<Color> colors;

    	// Skeleton
		Vector<Array<int, AnimationConstants::MaxBoneInfluence>> m_BoneIds;
		Vector<Array<float, AnimationConstants::MaxBoneInfluence>> m_Weights;
		Vector<int> m_BoneWeightCounters;

    	//Other
    	Vector<Ref<Mesh>> m_SubMeshes;
    	
		AABBBoundingVolume m_BoundingVolume{};
};
}
