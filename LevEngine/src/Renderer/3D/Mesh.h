#pragma once

#include "DataTypes/Map.h"
#include "DataTypes/Vector.h"
#include "Math/BoundingVolume.h"
#include "Math/Color.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Renderer/BufferBinding.h"

namespace LevEngine
{
	class Shader;
	class VertexBuffer;
	class IndexBuffer;

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

	[[nodiscard]] const Vector<uint32_t>& GetIndices() const { return indices; }
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
