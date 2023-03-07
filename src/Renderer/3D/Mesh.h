#pragma once

#include <memory>
#include "SimpleMath.h"

#include "../D3D11IndexBuffer.h"
#include "../D3D11VertexBuffer.h"

class Mesh
{
public:
	static std::shared_ptr<Mesh> CreatePlane(int resolution);
	static std::shared_ptr<Mesh> CreateSphere(const uint32_t sliceCount);
	static std::shared_ptr<Mesh> CreateCube();

	std::shared_ptr<D3D11IndexBuffer> CreateIndexBuffer() const;

	[[nodiscard]] std::shared_ptr<D3D11VertexBuffer> CreateVertexBuffer(const BufferLayout&) const;

	void Clear()
	{
		vertices.clear();
		uvs.clear();
		indices.clear();
		normals.clear();
	}

	[[nodiscard]] uint32_t GetVerticesCount() const { return vertices.size(); }
	[[nodiscard]] DirectX::SimpleMath::Vector3 GetVertex(uint32_t index) const { return vertices[index]; }
	void AddVertex(const DirectX::SimpleMath::Vector3& value) { vertices.emplace_back(value); }

	[[nodiscard]] uint32_t GetIndicesCount() const { return indices.size(); }
	[[nodiscard]] uint32_t GetIndex(uint32_t index) const { return indices[index]; }
	void AddTriangle(const DirectX::SimpleMath::Vector3& value)
	{
		indices.emplace_back(value.x);
		indices.emplace_back(value.y);
		indices.emplace_back(value.z);
	}

	void AddIndex(const uint32_t& value) { indices.emplace_back(value); }

	[[nodiscard]] DirectX::SimpleMath::Vector2 GetUV(uint32_t index) const { return uvs[index]; }
	void AddUV(const DirectX::SimpleMath::Vector2& value) { uvs.emplace_back(value); }

	[[nodiscard]] DirectX::SimpleMath::Vector3 GetNormal(uint32_t index) const { return normals[index]; }
	void AddNormal(const DirectX::SimpleMath::Vector3& value) { normals.emplace_back(value); }

private:
	std::vector<DirectX::SimpleMath::Vector3> vertices;
	std::vector<DirectX::SimpleMath::Vector2> uvs;
	std::vector<uint32_t> indices;
	std::vector<DirectX::SimpleMath::Vector3> normals;

};

