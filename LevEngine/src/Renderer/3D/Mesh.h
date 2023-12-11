#pragma once

#include "Math/BoundingVolume.h"
#include "Renderer/BufferBinding.h"
#include "BoneInfo.h"

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
        [[nodiscard]] Vector3 GetVertex(const uint32_t index) const { return vertices[index]; }
        void AddVertex(const Vector3& value) { vertices.emplace_back(value); }

        [[nodiscard]] uint32_t GetIndicesCount() const { return static_cast<uint32_t>(indices.size()); }
        [[nodiscard]] uint32_t GetIndex(const uint32_t index) const { return indices[index]; }
        void AddTriangle(const Vector3& value);
        void AddIndex(const uint32_t& value) { indices.emplace_back(value); }

        [[nodiscard]] Vector2 GetUV(const uint32_t index) const { return uvs[index]; }
        void AddUV(const Vector2& value) { uvs.emplace_back(value); }

        [[nodiscard]] Vector3 GetNormal(const uint32_t index) const { return normals[index]; }
        void AddNormal(const Vector3& value) { normals.emplace_back(value); }

        void AddTangent(Vector3 value) { tangents.emplace_back(value); }
        void AddBiTangent(Vector3 value) { biTangents.emplace_back(value); }

        void AddVertexBuffer(const BufferBinding& binding, const Ref<VertexBuffer>& buffer);
        [[nodiscard]] const Map<BufferBinding, Ref<VertexBuffer>>& GetVertexBuffers() const { return m_VertexBuffers; }

        void Init();

        void Bind(const Ref<Shader>& shader) const;

        void SetAABBBoundingVolume(const Vector3& min, const Vector3& max) { m_BoundingVolume = {min, max}; }
        [[nodiscard]] const AABBBoundingVolume& GetAABBBoundingVolume() { return m_BoundingVolume; }

        [[nodiscard]] bool IsOnFrustum(const Frustum& frustum, const Transform& meshTransform) const;

		[[nodiscard]] UnorderedMap<String, BoneInfo>& GetBoneInfoMap() { return m_BoneInfoMap; }
		
		[[nodiscard]] Vector<Array<float, AnimationConstants::MaxBoneInfluence>>& GetBoneWeights();

		[[nodiscard]] int& GetBoneCount() { return m_BoneCounter; }
		void SetBoneCount(int value) { m_BoneCounter = value; }

		void AddBoneWeight(int vertexIdx, int boneID, float weight);
		void ResizeBoneArrays(size_t size);
		void NormalizeBoneWeights();

        Ref<IndexBuffer> IndexBuffer;

    private:
        Map<BufferBinding, Ref<VertexBuffer>> m_VertexBuffers;

    	// Mesh vertex data
        Vector<Vector3> vertices;
        Vector<Vector2> uvs;
        Vector<uint32_t> indices;
        Vector<Vector3> normals;
        Vector<Vector3> tangents;
        Vector<Vector3> biTangents;
        Vector<Color> colors;

		Vector<Array<int, AnimationConstants::MaxBoneInfluence>> m_BoneIds;
		Vector<Array<float, AnimationConstants::MaxBoneInfluence>> m_Weights;
		Vector<int> m_BoneWeightCounters;
		UnorderedMap<String, BoneInfo> m_BoneInfoMap;
		int m_BoneCounter = 0;

		AABBBoundingVolume m_BoundingVolume{};
};
}
