#pragma once

#include "DataTypes/Map.h"
#include "DataTypes/Vector.h"
#include "Math/BoundingVolume.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Renderer/BufferBinding.h"
#include "Renderer/IndexBuffer.h"
#include "Renderer/Shader.h"
#include "Renderer/VertexBuffer.h"

namespace LevEngine
{
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

	static Ref<Mesh> CreatePlane(int resolution);
	static Ref<Mesh> CreateSphere(uint32_t sliceCount);
	static Ref<Mesh> CreateCube();

	Ref<IndexBuffer> CreateIndexBuffer() const;

	void Clear()
	{
		vertices.clear();
		uvs.clear();
		indices.clear();
		normals.clear();
	}

	[[nodiscard]] uint32_t GetVerticesCount() const { return vertices.size(); }
	[[nodiscard]] Vector3 GetVertex(const uint32_t index) const { return vertices[index]; }
	void AddVertex(const Vector3& value) { vertices.emplace_back(value); }

	[[nodiscard]] uint32_t GetIndicesCount() const { return indices.size(); }
	[[nodiscard]] uint32_t GetIndex(const uint32_t index) const { return indices[index]; }
	void AddTriangle(const Vector3& value)
	{
		indices.emplace_back(value.x);
		indices.emplace_back(value.y);
		indices.emplace_back(value.z);
	}

	void AddIndex(const uint32_t& value) { indices.emplace_back(value); }

	[[nodiscard]] Vector2 GetUV(const uint32_t index) const { return uvs[index]; }
	void AddUV(const Vector2& value) { uvs.emplace_back(value); }

	[[nodiscard]] Vector3 GetNormal(const uint32_t index) const { return normals[index]; }
	void AddNormal(const Vector3& value) { normals.emplace_back(value); }
	void AddVertexBuffer(const BufferBinding& binding, const Ref<VertexBuffer>& buffer)
	{
		m_VertexBuffers[binding] = buffer;
	}

	[[nodiscard]] const Map<BufferBinding, Ref<VertexBuffer>>& GetVertexBuffers() const
	{
		return m_VertexBuffers;
	}

	void Init()
	{
		IndexBuffer = CreateIndexBuffer();

		if (vertices.size())
		{
			const auto buffer = VertexBuffer::Create(&vertices[0].x, vertices.size(), sizeof Vector3);
			AddVertexBuffer(BufferBinding("POSITION", 0), buffer);
		}

		if (normals.size())
		{
			const auto buffer = VertexBuffer::Create(&normals[0].x, normals.size(), sizeof Vector3);
			AddVertexBuffer(BufferBinding("NORMAL", 0), buffer);
		}

		if (tangents.size() && biTangents.size())
		{
			const auto tangentsBuffer = VertexBuffer::Create(&tangents[0].x, tangents.size(), sizeof Vector3);
			AddVertexBuffer(BufferBinding("TANGENT", 0), tangentsBuffer);

			const auto biTangentsBuffer = VertexBuffer::Create(&biTangents[0].x, biTangents.size(), sizeof Vector3);
			AddVertexBuffer(BufferBinding("BINORMAL", 0), biTangentsBuffer);
		}

		if (colors.size())
		{
			const auto buffer = VertexBuffer::Create(colors[0].Raw(), colors.size(), sizeof Color);
			AddVertexBuffer(BufferBinding("COLOR", 0), buffer);
		}

		if (uvs.size())
		{
			const auto buffer = VertexBuffer::Create(&uvs[0].x, uvs.size(), sizeof Vector2);
			AddVertexBuffer(BufferBinding("TEXCOORD", 0), buffer);
		}
	}

	void Bind(const Ref<Shader>& shader) const
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

	void AddTangent(Vector3 value) { tangents.emplace_back(value); }
	void AddBiTangent(Vector3 value) { biTangents.emplace_back(value); }

	void SetAABBBoundingVolume(const Vector3& min, const Vector3& max)
	{
		m_BoundingVolume = {min, max};
	}

	[[nodiscard]] bool IsOnFrustum(const Frustum& frustum, const Transform& meshTransform) const
	{
		return m_BoundingVolume.IsOnFrustum(frustum, meshTransform);
	}

	Ref<IndexBuffer> IndexBuffer;

private:
	Map<BufferBinding, Ref<VertexBuffer>> m_VertexBuffers;

	Vector<Vector3> vertices;
	Vector<Vector2> uvs;
	Vector<uint32_t> indices;
	Vector<Vector3> normals;
	Vector<Vector3> tangents;
	Vector<Vector3> biTangents;
	Vector<Color> colors;

	AABBBoundingVolume m_BoundingVolume{};
};
}
