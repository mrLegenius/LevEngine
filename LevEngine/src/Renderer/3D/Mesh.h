#pragma once
#include <filesystem>

#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Renderer/IndexBuffer.h"
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
	Mesh() : Mesh("Default Mesh Name") { }
	explicit Mesh(std::filesystem::path path) : m_Name(path.stem().string()), m_Path(std::move(path)) { }

	static std::shared_ptr<Mesh> CreatePlane(int resolution);
	static std::shared_ptr<Mesh> CreateSphere(uint32_t sliceCount);
	static std::shared_ptr<Mesh> CreateCube();

	std::shared_ptr<IndexBuffer> CreateIndexBuffer() const;

	[[nodiscard]] std::shared_ptr<VertexBuffer> CreateVertexBuffer(const BufferLayout&) const;
	std::shared_ptr<VertexBuffer> CreateVertexBuffer(const BufferLayout& bufferLayout, float* data) const;

	[[nodiscard]] const std::string& GetName() const { return m_Name; }

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
	void Init(const BufferLayout& layout)
	{
		IndexBuffer = CreateIndexBuffer();

		const auto verticesCount = GetVerticesCount();
		const auto meshVertexBuffer = new MeshVertex[verticesCount];

		for (uint32_t i = 0; i < verticesCount; i++)
		{
			meshVertexBuffer[i].Position = GetVertex(i);
			meshVertexBuffer[i].Normal = GetNormal(i);
			meshVertexBuffer[i].UV = GetUV(i);
		}

		VertexBuffer = CreateVertexBuffer(layout, reinterpret_cast<float*>(meshVertexBuffer));

		delete[] meshVertexBuffer;
	}

	[[nodiscard]] const std::filesystem::path& GetPath() const { return m_Path; }

	Ref<IndexBuffer> IndexBuffer;
	Ref<VertexBuffer> VertexBuffer;

private:
	std::vector<Vector3> vertices;
	std::vector<Vector2> uvs;
	std::vector<uint32_t> indices;
	std::vector<Vector3> normals;
	std::string m_Name;
	std::filesystem::path m_Path;
};
}