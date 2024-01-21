#pragma once

#include "AnimationConstants.h"
#include "Math/BoundingVolume.h"
#include "Renderer/BufferBinding.h"
#include "cereal/access.hpp"
#include "DataTypes/Set.h"

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

    	[[nodiscard]] const Vector<Vector2>& GetUVs() const { return uvs; }
        [[nodiscard]] Vector2 GetUV(const uint32_t index) const { return uvs[index]; }
        void AddUV(const Vector2& value) { uvs.emplace_back(value); }

    	[[nodiscard]] const Vector<Vector3>& GetNormals() const { return normals; }
        [[nodiscard]] Vector3 GetNormal(const uint32_t index) const { return normals[index]; }
        void AddNormal(const Vector3& value) { normals.emplace_back(value); }

    	[[nodiscard]] const Vector<Vector3>& GetTangents() const { return tangents; }
        void AddTangent(Vector3 value) { tangents.emplace_back(value); }

    	[[nodiscard]] const Vector<Vector3>& GetBiTangents() const { return biTangents; }
        void AddBiTangent(Vector3 value) { biTangents.emplace_back(value); }

        void AddVertexBuffer(const BufferBinding& binding, const Ref<VertexBuffer>& buffer);
        [[nodiscard]] const Map<BufferBinding, Ref<VertexBuffer>>& GetVertexBuffers() const { return m_VertexBuffers; }

        void Init();

        void Bind(const Ref<Shader>& shader) const;

        void SetAABBBoundingVolume(const Vector3& min, const Vector3& max) { m_BoundingVolume = {min, max}; }
        void SetAABBBoundingVolume(const Vector3& center, const float i, const float j, const float k)
        {
	        m_BoundingVolume = {center, i, j, k};
        }
        [[nodiscard]] const AABBBoundingVolume& GetAABBBoundingVolume() { return m_BoundingVolume; }

        [[nodiscard]] bool IsOnFrustum(const Frustum& frustum, const Transform& meshTransform) const;

		[[nodiscard]] const Vector<Array<int, AnimationConstants::MaxBoneInfluence>>& GetBoneIds();
    	void AddBoneIDs(const Array<int, AnimationConstants::MaxBoneInfluence>& boneIDs);
    	
		[[nodiscard]] const Vector<Array<float, AnimationConstants::MaxBoneInfluence>>& GetBoneWeights();
    	void AddBoneWeights(const Array<float, AnimationConstants::MaxBoneInfluence>& boneWeights);
		void AddBoneWeight(int vertexIdx, int boneID, float weight);
		void ResizeBoneArrays(size_t size);
		void NormalizeBoneWeights();

    	void Serialize(const Path& path) const;
    	void Deserialize(const Path& path);

        Ref<IndexBuffer> IndexBuffer;

    private:
    	friend class cereal::access;
    	template <class Archive>
		void save(Archive& output) const
    	{
    		output(vertices);
    		output(uvs);
    		output(indices);
    		output(normals);
    		output(tangents);
    		output(biTangents);
    		output(m_BoneIds);
    		output(m_Weights);
    		output(m_BoneWeightCounters);
    		output(m_BoundingVolume.center);
    		output(m_BoundingVolume.extents);
    	}
      
    	template <class Archive>
		void load(Archive& input)
    	{
    		input(vertices);
    		input(uvs);
    		input(indices);
    		input(normals);
    		input(tangents);
    		input(biTangents);
    		input(m_BoneIds);
    		input(m_Weights);
    		input(m_BoneWeightCounters);
    		
    		Vector3 aabbCenter = m_BoundingVolume.center;
    		Vector3 aabbExtents = m_BoundingVolume.extents;
    		input(aabbCenter);
    		input(aabbExtents);
    		m_BoundingVolume = {aabbCenter, aabbExtents.x, aabbExtents.y, aabbExtents.z};
    	}
    	
        Map<BufferBinding, Ref<VertexBuffer>> m_VertexBuffers;

    	// Mesh vertex data
        Vector<Vector3> vertices;
        Vector<Vector2> uvs;
        Vector<uint32_t> indices;
        Vector<Vector3> normals;
        Vector<Vector3> tangents;
        Vector<Vector3> biTangents;

		Vector<Array<int, AnimationConstants::MaxBoneInfluence>> m_BoneIds;
		Vector<Array<float, AnimationConstants::MaxBoneInfluence>> m_Weights;
		Vector<int> m_BoneWeightCounters;

		AABBBoundingVolume m_BoundingVolume{};
};
}
