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

	static Ref<Mesh> CreatePlane(const uint32_t resolution);
	static Ref<Mesh> CreateWireCube();
	static Ref<Mesh> CreateLine(Vector3 start, Vector3 end);
	static Ref<Mesh> CreateSphere(uint32_t sliceCount);
	static Ref<Mesh> CreateRing(Vector3 majorAxis, Vector3 minorAxis);
	static Ref<Mesh> CreateCube();
	static Ref<Mesh> CreateGrid(Vector3 xAxis, Vector3 yAxis, uint32_t xDivisions, uint32_t yDivisions);


	Ref<IndexBuffer> CreateIndexBuffer() const;

	void Clear()
	{
		vertices.clear();
		uvs.clear();
		indices.clear();
		normals.clear();
	}

	[[nodiscard]] uint32_t GetVerticesCount() const { return static_cast<uint32_t>(vertices.size()); }
	[[nodiscard]] Vector3 GetVertex(const uint32_t index) const { return vertices[index]; }
	void AddVertex(const Vector3& value) { vertices.emplace_back(value); }

	[[nodiscard]] uint32_t GetIndicesCount() const { return static_cast<uint32_t>(indices.size()); }
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

	void SetAABBBoundingVolume(const Vector3& min, const Vector3& max) { m_BoundingVolume = {min, max}; }
	[[nodiscard]] const AABBBoundingVolume& GetAABBBoundingVolume() { return m_BoundingVolume; }

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
