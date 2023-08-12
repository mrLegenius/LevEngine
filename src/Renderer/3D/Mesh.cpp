#include "pch.h"
#include "Mesh.h"

namespace LevEngine
{
std::shared_ptr<Mesh> Mesh::CreatePlane(const int resolution)
{
	auto mesh = std::make_shared<Mesh>("Plane");

	const float size = 1.f / (resolution - 1);

	for (int i = 0; i < resolution; i++)
	{
		for (int j = 0; j < resolution; j++)
		{
			auto pos = Vector3(i * size - 0.5f, j * size - 0.5f, 0);
			mesh->vertices.emplace_back(pos);

			auto normal = Vector3(0, 0, -1);
			mesh->normals.emplace_back(normal);

			auto uv = Vector2(i * size, j * size);
			mesh->uvs.emplace_back(uv);

			if (i == resolution - 1 || j == resolution - 1) continue;

			auto triangle1 = Vector3(resolution * i + j,
			                         resolution * i + j + resolution,
			                         resolution * i + j + resolution + 1);

			auto triangle2 = Vector3(resolution * i + j,
			                         resolution * i + j + resolution + 1,
			                         resolution * i + j + 1);

			mesh->AddTriangle(triangle1);
			mesh->AddTriangle(triangle2);
		}
	}

	return mesh;
}

std::shared_ptr<Mesh> Mesh::CreateCube()
{
	auto mesh = std::make_shared<Mesh>("Cube");

	constexpr float side = 0.5f;

	// front

	mesh->vertices.emplace_back(Vector3(-side, -side, -side));
	mesh->normals.emplace_back(Vector3(0.0f, 0.0f, -1.0f));
	mesh->uvs.emplace_back(Vector2(0.0f, 0.0f));

	mesh->vertices.emplace_back(Vector3(-side, side, -side));
	mesh->normals.emplace_back(Vector3(0.0f, 0.0f, -1.0f));
	mesh->uvs.emplace_back(Vector2(0.0f, 1.0f));

	mesh->vertices.emplace_back(Vector3(side, side, -side));
	mesh->normals.emplace_back(Vector3(0.0f, 0.0f, -1.0f));
	mesh->uvs.emplace_back(Vector2(1.0f, 1.0f));

	mesh->vertices.emplace_back(Vector3(side, -side, -side));
	mesh->normals.emplace_back(Vector3(0.0f, 0.0f, -1.0f));
	mesh->uvs.emplace_back(Vector2(1.0f, 0.0f));

	// back
	mesh->vertices.emplace_back(Vector3(-side, -side, side));
	mesh->normals.emplace_back(Vector3(0.0f, 0.0f, 1.0f));
	mesh->uvs.emplace_back(Vector2(1.0f, 1.0f));

	mesh->vertices.emplace_back(Vector3(side, -side, side));
	mesh->normals.emplace_back(Vector3(0.0f, 0.0f, 1.0f));
	mesh->uvs.emplace_back(Vector2(0.0f, 1.0f));

	mesh->vertices.emplace_back(Vector3(side, side, side));
	mesh->normals.emplace_back(Vector3(0.0f, 0.0f, 1.0f));
	mesh->uvs.emplace_back(Vector2(0.0f, 0.0f));

	mesh->vertices.emplace_back(Vector3(-side, side, side));
	mesh->normals.emplace_back(Vector3(0.0f, 0.0f, 1.0f));
	mesh->uvs.emplace_back(Vector2(1.0f, 0.0f));

	//top
	mesh->vertices.emplace_back(Vector3(-side, side, -side));
	mesh->normals.emplace_back(Vector3(0.0f, 1.0f, 0.0f));
	mesh->uvs.emplace_back(Vector2(0.0f, 0.0f));

	mesh->vertices.emplace_back(Vector3(-side, side, side));
	mesh->normals.emplace_back(Vector3(0.0f, 1.0f, 0.0f));
	mesh->uvs.emplace_back(Vector2(1.0f, 0.0f));

	mesh->vertices.emplace_back(Vector3(side, side, side));
	mesh->normals.emplace_back(Vector3(0.0f, 1.0f, 0.0f));
	mesh->uvs.emplace_back(Vector2(1.0f, 1.0f));

	mesh->vertices.emplace_back(Vector3(side, side, -side));
	mesh->normals.emplace_back(Vector3(0.0f, 1.0f, 0.0f));
	mesh->uvs.emplace_back(Vector2(0.0f, 1.0f));

	//bottom
	mesh->vertices.emplace_back(Vector3(-side, -side, -side));
	mesh->normals.emplace_back(Vector3(0.0f, -1.0f, 0.0f));
	mesh->uvs.emplace_back(Vector2(1.0f, 1.0f));

	mesh->vertices.emplace_back(Vector3(side, -side, -side));
	mesh->normals.emplace_back(Vector3(0.0f, -1.0f, 0.0f));
	mesh->uvs.emplace_back(Vector2(0.0f, 1.0f));

	mesh->vertices.emplace_back(Vector3(side, -side, side));
	mesh->normals.emplace_back(Vector3(0.0f, -1.0f, 0.0f));
	mesh->uvs.emplace_back(Vector2(0.0f, 0.0f));

	mesh->vertices.emplace_back(Vector3(-side, -side, side));
	mesh->normals.emplace_back(Vector3(0.0f, -1.0f, 0.0f));
	mesh->uvs.emplace_back(Vector2(1.0f, 0.0f));

	// left
	mesh->vertices.emplace_back(Vector3(-side, -side, side));
	mesh->normals.emplace_back(Vector3(-1.0f, 0.0f, 0.0f));
	mesh->uvs.emplace_back(Vector2(0.0f, 1.0f));

	mesh->vertices.emplace_back(Vector3(-side, side, side));
	mesh->normals.emplace_back(Vector3(-1.0f, 0.0f, 0.0f));
	mesh->uvs.emplace_back(Vector2(0.0f, 0.0f));

	mesh->vertices.emplace_back(Vector3(-side, side, -side));
	mesh->normals.emplace_back(Vector3(-1.0f, 0.0f, 0.0f));
	mesh->uvs.emplace_back(Vector2(1.0f, 0.0f));

	mesh->vertices.emplace_back(Vector3(-side, -side, -side));
	mesh->normals.emplace_back(Vector3(-1.0f, 0.0f, 0.0f));
	mesh->uvs.emplace_back(Vector2(1.0f, 1.0f));

	// right
	mesh->vertices.emplace_back(Vector3(side, -side, -side));
	mesh->normals.emplace_back(Vector3(1.0f, 0.0f, 0.0f));
	mesh->uvs.emplace_back(Vector2(0.0f, 1.0f));

	mesh->vertices.emplace_back(Vector3(side, side, -side));
	mesh->normals.emplace_back(Vector3(1.0f, 0.0f, 0.0f));
	mesh->uvs.emplace_back(Vector2(0.0f, 0.0f));

	mesh->vertices.emplace_back(Vector3(side, side, side));
	mesh->normals.emplace_back(Vector3(1.0f, 0.0f, 0.0f));
	mesh->uvs.emplace_back(Vector2(1.0f, 0.0f));

	mesh->vertices.emplace_back(Vector3(side, -side, side));
	mesh->normals.emplace_back(Vector3(1.0f, 0.0f, 0.0f));
	mesh->uvs.emplace_back(Vector2(1.0f, 1.0f));

	//indices
	mesh->AddTriangle(Vector3(0, 1, 2));
	mesh->AddTriangle(Vector3(0, 2, 3));
	mesh->AddTriangle(Vector3(4, 5, 6));
	mesh->AddTriangle(Vector3(4, 6, 7));
	mesh->AddTriangle(Vector3(8, 9, 10));
	mesh->AddTriangle(Vector3(8, 10, 11));
	mesh->AddTriangle(Vector3(12, 13, 14));
	mesh->AddTriangle(Vector3(12, 14, 15));
	mesh->AddTriangle(Vector3(16, 17, 18));
	mesh->AddTriangle(Vector3(16, 18, 19));
	mesh->AddTriangle(Vector3(20, 21, 22));
	mesh->AddTriangle(Vector3(20, 22, 23));

	return mesh;
}

std::shared_ptr<Mesh> Mesh::CreateSphere(const uint32_t sliceCount)
{
	assert(sliceCount >= 3u);

	const uint32_t stackCount = sliceCount / 2;
	const auto phiStep = DirectX::XM_PI / stackCount;
	const auto thetaStep = DirectX::XM_2PI / sliceCount;

	auto mesh = std::make_shared<Mesh>("Sphere");

	mesh->vertices.emplace_back(Vector3(0, 1, 0));
	mesh->normals.emplace_back(Vector3(0, 1, 0));
	mesh->uvs.emplace_back(Vector2(0, 1));

	for (int i = 1; i <= stackCount - 1; i++) 
	{
		const auto phi = i * phiStep;
		for (int j = 0; j <= sliceCount; j++) 
		{
			const auto theta = j * thetaStep;

			const float x = std::sin(phi) * std::sin(theta);
			const float y = std::cos(phi);
			const float z = std::sin(phi) * std::cos(theta);

			auto pos = Vector3(x, y, z);
			mesh->vertices.emplace_back(pos);

			pos.Normalize();
			mesh->normals.emplace_back(pos);

			auto uv = Vector2(static_cast<float>(j) / sliceCount, 1 - static_cast<float>(i) / stackCount);
			mesh->uvs.emplace_back(uv);
		}
	}

	mesh->vertices.emplace_back(Vector3(0, -1, 0));
	mesh->normals.emplace_back(Vector3(0, -1, 0));
	mesh->uvs.emplace_back(Vector2(0, 0));

	for (int i = 1; i <= sliceCount; i++) 
	{
		auto triangle = Vector3(i, i + 1, 0);
		mesh->AddTriangle(triangle);
	}

	auto baseIndex = 1;
	const auto ringVertexCount = sliceCount + 1;
	for (int i = 0; i < stackCount - 2; i++) {
		for (int j = 0; j < sliceCount; j++) {

			auto triangle2 = Vector3(baseIndex + (i + 1) * ringVertexCount + j + 1,
				baseIndex + i * ringVertexCount + j + 1,
				baseIndex + (i + 1) * ringVertexCount + j);

			auto triangle1 = Vector3(baseIndex + (i + 1) * ringVertexCount + j,
			                         baseIndex + i * ringVertexCount + j + 1,
									 baseIndex + i * ringVertexCount + j);

			mesh->AddTriangle(triangle1);
			mesh->AddTriangle(triangle2);
		}
	}

	const auto southPoleIndex = mesh->GetVerticesCount() - 1;
	baseIndex = southPoleIndex - ringVertexCount;
	for (int i = 0; i < sliceCount; i++) {

		auto triangle = Vector3(baseIndex + i + 1, baseIndex + i, southPoleIndex);
		mesh->AddTriangle(triangle);
	}

	return mesh;
}

std::shared_ptr<IndexBuffer> Mesh::CreateIndexBuffer() const
{
	auto indicesCount = GetIndicesCount();
	const auto indices = new uint32_t[indicesCount];
	for (uint32_t i = 0; i < indicesCount; i++)
		indices[i] = GetIndex(i);

	auto indexBuffer = IndexBuffer::Create(indices, indicesCount);
	delete[] indices;

	return indexBuffer;
}

std::shared_ptr<VertexBuffer> Mesh::CreateVertexBuffer(const BufferLayout& bufferLayout) const
{
	auto vertexBufferSize = GetVerticesCount() * bufferLayout.GetStride();
	auto vertexBuffer = VertexBuffer::Create(vertexBufferSize);

	vertexBuffer->SetLayout(bufferLayout);

	return vertexBuffer;
}

std::shared_ptr<VertexBuffer> Mesh::CreateVertexBuffer(const BufferLayout& bufferLayout, float* data) const
{
	auto vertexBufferSize = GetVerticesCount() * bufferLayout.GetStride();
	auto vertexBuffer = VertexBuffer::Create(data, vertexBufferSize);

	vertexBuffer->SetLayout(bufferLayout);

	return vertexBuffer;
}
}