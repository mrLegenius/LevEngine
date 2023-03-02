#pragma once

#include <memory>
#include "SimpleMath.h"

#include "../D3D11IndexBuffer.h"
#include "../D3D11VertexBuffer.h"

class Mesh
{
public:
	static std::shared_ptr<Mesh> CreatePlane(int resolution);
	static std::shared_ptr<Mesh> CreateSphere(const uint32_t numberSlices);
	static std::shared_ptr<Mesh> CreateCube();

	std::shared_ptr<D3D11IndexBuffer> CreateIndexBuffer() const;

	[[nodiscard]] std::shared_ptr<D3D11VertexBuffer> CreateVertexBuffer(const BufferLayout&) const;

	void Clear()
	{
		vertices.clear();
		uvs.clear();
		triangles.clear();
		normals.clear();
	}

	[[nodiscard]] uint32_t GetVerticesCount() const { return vertices.size(); }
	[[nodiscard]] DirectX::SimpleMath::Vector3 GetVertex(uint32_t index) const { return vertices[index]; }

	[[nodiscard]] uint32_t GetTrianglesCount() const { return triangles.size(); }
	[[nodiscard]] DirectX::SimpleMath::Vector3 GetTriangle(uint32_t index) const { return triangles[index]; }

	[[nodiscard]] DirectX::SimpleMath::Vector2 GetUV(uint32_t index) const { return uvs[index]; }
	[[nodiscard]] DirectX::SimpleMath::Vector3 GetNormal(uint32_t index) const { return normals[index]; }

private:
	std::vector<DirectX::SimpleMath::Vector3> vertices;
	std::vector<DirectX::SimpleMath::Vector2> uvs;
	std::vector<DirectX::SimpleMath::Vector3> triangles;
	std::vector<DirectX::SimpleMath::Vector3> normals;
};

