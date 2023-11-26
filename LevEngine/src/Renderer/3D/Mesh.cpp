#include "levpch.h"
#include "Mesh.h"

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
		const auto buffer = VertexBuffer::Create(&vertices[0].x, static_cast<uint32_t>(vertices.size()), sizeof Vector3);
		AddVertexBuffer(BufferBinding("POSITION", 0), buffer);
	}

	if (normals.size())
	{
		const auto buffer = VertexBuffer::Create(&normals[0].x, static_cast<uint32_t>(normals.size()), sizeof Vector3);
		AddVertexBuffer(BufferBinding("NORMAL", 0), buffer);
	}

	if (tangents.size() && biTangents.size())
	{
		const auto tangentsBuffer = VertexBuffer::Create(&tangents[0].x, static_cast<uint32_t>(tangents.size()), sizeof Vector3);
		AddVertexBuffer(BufferBinding("TANGENT", 0), tangentsBuffer);

		const auto biTangentsBuffer = VertexBuffer::Create(&biTangents[0].x, static_cast<uint32_t>(biTangents.size()), sizeof Vector3);
		AddVertexBuffer(BufferBinding("BINORMAL", 0), biTangentsBuffer);
	}

	if (colors.size())
	{
		const auto buffer = VertexBuffer::Create(colors[0].Raw(), static_cast<uint32_t>(colors.size()), sizeof Color);
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
			sizeof(Array<float, AnimationConstants::MaxBoneInfluence>));

		AddVertexBuffer(BufferBinding("BONEIDS", 0), buffer);
	}

	if (m_Weights.size())
	{
		const auto buffer = VertexBuffer::Create(&m_Weights[0][0], static_cast<uint32_t>(m_Weights.size()),
			sizeof(Array<float, AnimationConstants::MaxBoneInfluence>));

		AddVertexBuffer(BufferBinding("BONEWEIGHTS", 0), buffer);
	}
}

void Mesh::Bind(const Ref<Shader>& shader) const
{
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

void Mesh::SetVertexBoneDataToDefault(int vertexIdx)
{
	for (int i = 0; i < AnimationConstants::MaxBoneInfluence; i++)
	{
		while (m_BoneIds.size() <= vertexIdx)
		{
			m_BoneIds.emplace_back(Array<float, AnimationConstants::MaxBoneInfluence>());
		}

		while (m_Weights.size() <= vertexIdx)
		{
			m_Weights.emplace_back(Array<float, AnimationConstants::MaxBoneInfluence>());
		}
		
		m_BoneIds[vertexIdx][i] = -1;
		m_Weights[vertexIdx][i] = 0.0f;
	}
}

void Mesh::SetVertexBoneData(int vertexIdx, int boneID, float weight)
{
	for (int i = 0; i < AnimationConstants::MaxBoneInfluence; ++i)
	{
		if (m_BoneIds[vertexIdx][i] < 0)
		{
			m_Weights[vertexIdx][i] = weight;
			m_BoneIds[vertexIdx][i] = boneID;
			break;
		}
	}
}

}
