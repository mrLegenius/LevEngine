#include "levpch.h"
#include "Primitives.h"

#include "Mesh.h"
#include "Math/Math.h"

namespace LevEngine
{

Ref<Mesh> Primitives::CreatePlane(const uint32_t resolution)
{
	auto mesh = CreateRef<Mesh>();

	const float size = 1.f / static_cast<float>(resolution - 1);

	for (uint32_t i = 0; i < resolution; i++)
	{
		for (uint32_t j = 0; j < resolution; j++)
		{
			auto pos = Vector3(static_cast<float>(i) * size - 0.5f, static_cast<float>(j) * size - 0.5f, 0);
			mesh->AddVertex(pos);

			auto normal = Vector3(0, 0, -1);
			mesh->AddNormal(normal);

			auto uv = Vector2(static_cast<float>(i) * size, static_cast<float>(j) * size);
			mesh->AddUV(uv);

			if (i == resolution - 1 || j == resolution - 1) continue;

			mesh->AddIndex(resolution * i + j);
			mesh->AddIndex(resolution * i + j + resolution);
			mesh->AddIndex(resolution * i + j + resolution + 1);
			
			mesh->AddIndex(resolution * i + j);
			mesh->AddIndex(resolution * i + j + resolution + 1);
			mesh->AddIndex(resolution * i + j + 1);
		}
	}

	mesh->Init();

	return mesh;
}

Ref<Mesh> Primitives::CreateWireCube()
{
	auto mesh = CreateRef<Mesh>();

	for (int i = 0; i < 8; i++) 
	{
		mesh->AddVertex(Vector3(
			static_cast<float>(i & 1) - 0.5f,
			static_cast<float>((i >> 1) & 1) - 0.5f,
			static_cast<float>(i >> 2) - 0.5f));
	}

	for (int i = 0; i < 4; i++)
	{
		mesh->AddIndex(2 * i);
		mesh->AddIndex(2 * i + 1);

		mesh->AddIndex(i);
		mesh->AddIndex(i + 4);

		if (i < 2)
		{
			mesh->AddIndex(i);
			mesh->AddIndex(i + 2);
		}
		else
		{
			mesh->AddIndex(i + 2);
			mesh->AddIndex(i + 4);
		}
	}

	mesh->Init();

	return mesh;
}

Ref<Mesh> Primitives::CreateLine(const Vector3 start, const Vector3 end)
{
	auto mesh = CreateRef<Mesh>();

	mesh->AddVertex(start);
	mesh->AddVertex(end);
	mesh->AddIndex(0);
	mesh->AddIndex(1);

	mesh->Init();

	return mesh;
}
	
Ref<Mesh> Primitives::CreateCube()
{
	auto mesh = CreateRef<Mesh>();

	constexpr float side = 0.5f;

	// front

	mesh->AddVertex(Vector3(-side, -side, -side));
	mesh->AddNormal(Vector3(0.0f, 0.0f, -1.0f));
	mesh->AddUV(Vector2(0.0f, 0.0f));

	mesh->AddVertex(Vector3(-side, side, -side));
	mesh->AddNormal(Vector3(0.0f, 0.0f, -1.0f));
	mesh->AddUV(Vector2(0.0f, 1.0f));

	mesh->AddVertex(Vector3(side, side, -side));
	mesh->AddNormal(Vector3(0.0f, 0.0f, -1.0f));
	mesh->AddUV(Vector2(1.0f, 1.0f));

	mesh->AddVertex(Vector3(side, -side, -side));
	mesh->AddNormal(Vector3(0.0f, 0.0f, -1.0f));
	mesh->AddUV(Vector2(1.0f, 0.0f));

	// back
	mesh->AddVertex(Vector3(-side, -side, side));
	mesh->AddNormal(Vector3(0.0f, 0.0f, 1.0f));
	mesh->AddUV(Vector2(1.0f, 1.0f));

	mesh->AddVertex(Vector3(side, -side, side));
	mesh->AddNormal(Vector3(0.0f, 0.0f, 1.0f));
	mesh->AddUV(Vector2(0.0f, 1.0f));

	mesh->AddVertex(Vector3(side, side, side));
	mesh->AddNormal(Vector3(0.0f, 0.0f, 1.0f));
	mesh->AddUV(Vector2(0.0f, 0.0f));

	mesh->AddVertex(Vector3(-side, side, side));
	mesh->AddNormal(Vector3(0.0f, 0.0f, 1.0f));
	mesh->AddUV(Vector2(1.0f, 0.0f));

	//top
	mesh->AddVertex(Vector3(-side, side, -side));
	mesh->AddNormal(Vector3(0.0f, 1.0f, 0.0f));
	mesh->AddUV(Vector2(0.0f, 0.0f));

	mesh->AddVertex(Vector3(-side, side, side));
	mesh->AddNormal(Vector3(0.0f, 1.0f, 0.0f));
	mesh->AddUV(Vector2(1.0f, 0.0f));

	mesh->AddVertex(Vector3(side, side, side));
	mesh->AddNormal(Vector3(0.0f, 1.0f, 0.0f));
	mesh->AddUV(Vector2(1.0f, 1.0f));

	mesh->AddVertex(Vector3(side, side, -side));
	mesh->AddNormal(Vector3(0.0f, 1.0f, 0.0f));
	mesh->AddUV(Vector2(0.0f, 1.0f));

	//bottom
	mesh->AddVertex(Vector3(-side, -side, -side));
	mesh->AddNormal(Vector3(0.0f, -1.0f, 0.0f));
	mesh->AddUV(Vector2(1.0f, 1.0f));

	mesh->AddVertex(Vector3(side, -side, -side));
	mesh->AddNormal(Vector3(0.0f, -1.0f, 0.0f));
	mesh->AddUV(Vector2(0.0f, 1.0f));

	mesh->AddVertex(Vector3(side, -side, side));
	mesh->AddNormal(Vector3(0.0f, -1.0f, 0.0f));
	mesh->AddUV(Vector2(0.0f, 0.0f));

	mesh->AddVertex(Vector3(-side, -side, side));
	mesh->AddNormal(Vector3(0.0f, -1.0f, 0.0f));
	mesh->AddUV(Vector2(1.0f, 0.0f));

	// left
	mesh->AddVertex(Vector3(-side, -side, side));
	mesh->AddNormal(Vector3(-1.0f, 0.0f, 0.0f));
	mesh->AddUV(Vector2(0.0f, 1.0f));

	mesh->AddVertex(Vector3(-side, side, side));
	mesh->AddNormal(Vector3(-1.0f, 0.0f, 0.0f));
	mesh->AddUV(Vector2(0.0f, 0.0f));

	mesh->AddVertex(Vector3(-side, side, -side));
	mesh->AddNormal(Vector3(-1.0f, 0.0f, 0.0f));
	mesh->AddUV(Vector2(1.0f, 0.0f));

	mesh->AddVertex(Vector3(-side, -side, -side));
	mesh->AddNormal(Vector3(-1.0f, 0.0f, 0.0f));
	mesh->AddUV(Vector2(1.0f, 1.0f));

	// right
	mesh->AddVertex(Vector3(side, -side, -side));
	mesh->AddNormal(Vector3(1.0f, 0.0f, 0.0f));
	mesh->AddUV(Vector2(0.0f, 1.0f));

	mesh->AddVertex(Vector3(side, side, -side));
	mesh->AddNormal(Vector3(1.0f, 0.0f, 0.0f));
	mesh->AddUV(Vector2(0.0f, 0.0f));

	mesh->AddVertex(Vector3(side, side, side));
	mesh->AddNormal(Vector3(1.0f, 0.0f, 0.0f));
	mesh->AddUV(Vector2(1.0f, 0.0f));

	mesh->AddVertex(Vector3(side, -side, side));
	mesh->AddNormal(Vector3(1.0f, 0.0f, 0.0f));
	mesh->AddUV(Vector2(1.0f, 1.0f));

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

	mesh->Init();

	return mesh;
}

Ref<Mesh> Primitives::CreateGrid(const Vector3 xAxis, const Vector3 yAxis, const uint32_t xDivisions, const uint32_t yDivisions)
{
	auto mesh = CreateRef<Mesh>();

	const auto xdivs = Math::Max(xDivisions, 1u);
	const auto ydivs = Math::Max(yDivisions, 1u);

	uint32_t indices = 0;
	for (size_t i = 0; i <= xdivs; ++i)
	{
		float percent = static_cast<float>(i) / static_cast<float>(xdivs);
		percent = percent * 2.f - 1.f;
		Vector3 scale = xAxis * percent;
		Vector3 v1 = scale - yAxis;
		Vector3 v2 = scale + yAxis;

		mesh->AddVertex(v1);
		mesh->AddVertex(v2);

		mesh->AddIndex(indices++);
		mesh->AddIndex(indices++);
	}

	for (size_t i = 0; i <= ydivs; i++)
	{
		FLOAT percent = static_cast<float>(i) / static_cast<float>(ydivs);
		percent = (percent * 2.f) - 1.f;
		Vector3 scale = XMVectorScale(yAxis, percent);

		Vector3 v1 = scale - xAxis;
		Vector3 v2 = scale + xAxis;
		
		mesh->AddVertex(v1);
		mesh->AddVertex(v2);

		mesh->AddIndex(indices++);
		mesh->AddIndex(indices++);
	}

	mesh->Init();

	return mesh;
}

Ref<Mesh> Primitives::CreateSphere(const uint32_t sliceCount)
{
	assert(sliceCount >= 3u);

	const uint32_t stackCount = sliceCount / 2;
	const auto phiStep = DirectX::XM_PI / static_cast<float>(stackCount);
	const auto thetaStep = DirectX::XM_2PI / static_cast<float>(sliceCount);

	auto mesh = CreateRef<Mesh>();

	mesh->AddVertex(Vector3(0, 1, 0));
	mesh->AddNormal(Vector3(0, 1, 0));
	mesh->AddUV(Vector2(0, 1));

	for (uint32_t i = 1; i <= stackCount - 1; i++) 
	{
		const auto phi = static_cast<float>(i) * phiStep;
		for (uint32_t j = 0; j <= sliceCount; j++) 
		{
			const auto theta = static_cast<float>(j) * thetaStep;

			const float x = std::sin(phi) * std::sin(theta);
			const float y = std::cos(phi);
			const float z = std::sin(phi) * std::cos(theta);

			auto pos = Vector3(x, y, z);
			mesh->AddVertex(pos);

			pos.Normalize();
			mesh->AddNormal(pos);

			auto uv = Vector2(
				static_cast<float>(j) / static_cast<float>(sliceCount),
				1 - static_cast<float>(i) / static_cast<float>(stackCount));
			mesh->AddUV(uv);
		}
	}

	mesh->AddVertex(Vector3(0, -1, 0));
	mesh->AddNormal(Vector3(0, -1, 0));
	mesh->AddUV(Vector2(0, 0));

	for (uint32_t i = 1; i <= sliceCount; i++) 
	{
		mesh->AddIndex(i);
		mesh->AddIndex(i + 1);
		mesh->AddIndex(0);
	}

	uint32_t baseIndex = 1;
	const auto ringVertexCount = sliceCount + 1;
	for (uint32_t i = 0; i < stackCount - 2; i++) {
		for (uint32_t j = 0; j < sliceCount; j++) {

			mesh->AddIndex(baseIndex + (i + 1) * ringVertexCount + j);
			mesh->AddIndex(baseIndex + i * ringVertexCount + j + 1);
			mesh->AddIndex(baseIndex + i * ringVertexCount + j);
			
			mesh->AddIndex(baseIndex + (i + 1) * ringVertexCount + j + 1);
			mesh->AddIndex(baseIndex + i * ringVertexCount + j + 1);
			mesh->AddIndex(baseIndex + (i + 1) * ringVertexCount + j);
		}
	}

	const auto southPoleIndex = mesh->GetVerticesCount() - 1;
	baseIndex = southPoleIndex - ringVertexCount;
	for (uint32_t i = 0; i < sliceCount; i++)
	{
		mesh->AddIndex(baseIndex + i + 1);
		mesh->AddIndex(baseIndex + i);
		mesh->AddIndex(southPoleIndex);
	}

	mesh->Init();

	return mesh;
}

Ref<Mesh> Primitives::CreateRing(const Vector3 majorAxis, const Vector3 minorAxis)
{
	const auto mesh = CreateRef<Mesh>();
	
	constexpr size_t ringSegments = 32;
	constexpr float fAngleDelta = 2 * Math::Pi / static_cast<float>(ringSegments);

	const auto cosDelta = Vector3(cosf(fAngleDelta));
	const auto sinDelta = Vector3(sinf(fAngleDelta));

	Vector3 sin = Vector3::Zero;
	Vector3 cos = Vector3::One;
	
	for (size_t i = 0; i <= ringSegments; i++)
	{
		Vector3 pos = majorAxis * cos + minorAxis * sin;
		mesh->AddVertex(pos);

		const auto newCos = cos * cosDelta - sin * sinDelta;
		const auto newSin = cos * sinDelta + sin * cosDelta;

		cos = newCos;
		sin = newSin;
	}

	mesh->Init();

	return mesh;
}

}