#include "levpch.h"
#include "Mesh.h"

#include "Renderer/Pipeline/IndexBuffer.h"
#include "Renderer/Shader/Shader.h"
#include "Renderer/Pipeline/VertexBuffer.h"

namespace LevEngine
{
    Ref<IndexBuffer> Mesh::CreateIndexBuffer() const
    {
        const auto indicesCount = GetIndicesCount();
        const auto indices = new uint32_t[indicesCount];
        for (uint32_t i = 0; i < indicesCount; i++)
            indices[i] = GetIndex(i);

        auto indexBuffer = IndexBuffer::Create(indices, indicesCount);
        delete[] indices;

        return indexBuffer;
    }

    void Mesh::Clear()
    {
        vertices.clear();
        uvs.clear();
        indices.clear();
        normals.clear();
    }

    void Mesh::AddTriangle(const Vector3& value)
    {
        indices.emplace_back(value.x);
        indices.emplace_back(value.y);
        indices.emplace_back(value.z);
    }

    void Mesh::AddVertexBuffer(const BufferBinding& binding, const Ref<VertexBuffer>& buffer)
    {
        m_VertexBuffers[binding] = buffer;
    }

    void Mesh::Init()
    {
        if (indices.size())
            IndexBuffer = CreateIndexBuffer();

        if (vertices.size())
        {
            const auto buffer = VertexBuffer::Create(&vertices[0].x, static_cast<uint32_t>(vertices.size()),
                                                     sizeof Vector3);
            AddVertexBuffer(BufferBinding("POSITION", 0), buffer);
        }

        if (normals.size())
        {
            const auto buffer = VertexBuffer::Create(&normals[0].x, static_cast<uint32_t>(normals.size()),
                                                     sizeof Vector3);
            AddVertexBuffer(BufferBinding("NORMAL", 0), buffer);
        }

        if (tangents.size())
        {
            const auto tangentsBuffer = VertexBuffer::Create(&tangents[0].x, static_cast<uint32_t>(tangents.size()),
                                                             sizeof Vector3);
            AddVertexBuffer(BufferBinding("TANGENT", 0), tangentsBuffer);
        }

        if (colors.size())
        {
            const auto buffer = VertexBuffer::Create(colors[0].Raw(), static_cast<uint32_t>(colors.size()),
                                                     sizeof Color);
            AddVertexBuffer(BufferBinding("COLOR", 0), buffer);
        }

        if (uvs.size())
        {
            const auto buffer = VertexBuffer::Create(&uvs[0].x, static_cast<uint32_t>(uvs.size()), sizeof Vector2);
            AddVertexBuffer(BufferBinding("TEXCOORD", 0), buffer);
        }

        if (m_BoneIds.size())
        {
            const auto buffer = VertexBuffer::Create(&m_BoneIds[0][0], static_cast<uint32_t>(m_BoneIds.size()),
                                                     sizeof(int) * AnimationConstants::MaxBoneInfluence);
        
            AddVertexBuffer(BufferBinding("BONEIDS", 0), buffer);
        }
        
        if (m_Weights.size())
        {
            const auto buffer = VertexBuffer::Create(&m_Weights[0][0], static_cast<uint32_t>(m_Weights.size()),
                                                     sizeof(float) * AnimationConstants::MaxBoneInfluence);
        
            AddVertexBuffer(BufferBinding("BONEWEIGHTS", 0), buffer);
        }
    }

    void Mesh::GenerateAABBBoundingVolume()
    {
        Vector3 aabbMin, aabbMax;
        for (unsigned int vertexIdx = 0; vertexIdx < GetVerticesCount(); ++vertexIdx)
        {
            Vector3 position = GetVertex(vertexIdx);

            aabbMin = Vector3::Min(aabbMin, position);
            aabbMax = Vector3::Max(aabbMax, position);
        }

        SetAABBBoundingVolume(aabbMin, aabbMax);
    }

    void Mesh::Bind(const Ref<Shader>& shader) const
    {
        LEV_PROFILE_FUNCTION();
        
        for (auto [binding, buffer] : m_VertexBuffers)
        {
            if (shader->HasSemantic(binding))
            {
                const uint32_t slotId = shader->GetSlotIdBySemantic(binding);
                // Bind the vertex buffer to a particular slot ID.
                buffer->Bind(slotId);
            }
        }
    }

    bool Mesh::IsOnFrustum(const Frustum& frustum, const Transform& meshTransform) const
    {
        return m_BoundingVolume.IsOnFrustum(frustum, meshTransform);
    }

    Vector<Array<float, AnimationConstants::MaxBoneInfluence>>& Mesh::GetBoneWeights()
    {
        return m_Weights;
    }

    void Mesh::AddBoneWeight(int vertexIdx, int boneID, float weight)
    {
        const int weightsCount = m_BoneWeightCounters[vertexIdx];

        if (weightsCount < AnimationConstants::MaxBoneInfluence)
        {
            m_BoneIds[vertexIdx][weightsCount] = boneID;
            m_Weights[vertexIdx][weightsCount] = weight;
            m_BoneWeightCounters[vertexIdx]++;
        }
        else
        {
            int minBoneId = -1;
            float minBoneWeight = FLT_MAX;

            for (int i = 0; i < AnimationConstants::MaxBoneInfluence; ++i)
            {
                if (minBoneId == -1 || m_Weights[vertexIdx][i] < minBoneWeight)
                {
                    minBoneId = i;
                    minBoneWeight = m_Weights[vertexIdx][i];
                }
            }

            if (weight > minBoneWeight)
            {
                m_BoneIds[vertexIdx][minBoneId] = boneID;
                m_Weights[vertexIdx][minBoneId] = weight;
            }
        }
    }

    void Mesh::ResizeBoneArrays(size_t size)
    {
        m_BoneIds.resize(size, Array<int, AnimationConstants::MaxBoneInfluence>());
        m_Weights.resize(size, Array<float, AnimationConstants::MaxBoneInfluence>());
        m_BoneWeightCounters.resize(size, 0);
    }

    void Mesh::NormalizeBoneWeights()
    {
        for (int vertexIdx = 0; vertexIdx < GetVerticesCount(); ++vertexIdx)
        {
            float totalWeight = 0.0f;
            for (int weightIdx = 0; weightIdx < AnimationConstants::MaxBoneInfluence; ++weightIdx)
            {
                totalWeight += m_Weights[vertexIdx][weightIdx];
            }

            if (totalWeight > 0.0f)
            {
                for (int weightIdx = 0; weightIdx < AnimationConstants::MaxBoneInfluence; ++weightIdx)
                {
                    m_Weights[vertexIdx][weightIdx] /= totalWeight;
                }
            }
            else
            {
                m_Weights[vertexIdx][0] = 1.0f;
            }
        }
    }
}
