#pragma once
#include <memory>

#include "RendererComponent.h"
#include "Transform.h"
#include "../Renderer/D3D11VertexBuffer.h"
#include "../Renderer/D3D11IndexBuffer.h"
#include "../Renderer/D3D11Shader.h"

struct ShapeVertex
{
	DirectX::SimpleMath::Vector4 position;
	DirectX::SimpleMath::Vector4 color;
};

class ShapeRenderer : public RendererComponent
{
public:
	ShapeRenderer(const std::shared_ptr<D3D11Shader>& shader, const int edges)
		: RendererComponent(shader),
		m_VerticesCount(edges + 1),
		m_VertexPositions(new DirectX::SimpleMath::Vector4[m_VerticesCount]),
		m_VerticesData(new ShapeVertex[m_VerticesCount])
	{
		auto size = m_VerticesCount * sizeof(ShapeVertex);
		m_VertexBuffer = std::make_shared<D3D11VertexBuffer>(size);
		m_VertexBuffer->SetLayout(m_Shader->GetLayout());

		int indicesCount = edges * 3;
		auto indices = new unsigned int[indicesCount];

		m_VertexPositions[0].x = 0.0f;
		m_VertexPositions[0].y = 0.0f;
		m_VertexPositions[0].z = 0.5f;
		m_VertexPositions[0].w = 1.0f;

		for (int i = 1; i < m_VerticesCount; i++)
		{
			m_VertexPositions[i].x = cos(DirectX::XM_2PI * i / edges);
			m_VertexPositions[i].y = sin(DirectX::XM_2PI * i / edges);
			m_VertexPositions[i].z = 0.5f;
			m_VertexPositions[i].w = 1.0f;

			indices[i * 3 - 3] = 0;
			indices[i * 3 - 2] = i;
			indices[i * 3 - 1] = i + 1;
		}

		indices[indicesCount - 1] = 1;

		m_IndexBuffer = std::make_shared<D3D11IndexBuffer>(indices, indicesCount);

		delete[] indices;
	}

	void Prepare(const std::shared_ptr<Transform>& transform) override
	{
		for (int i = 0; i < m_VerticesCount; ++i)
		{
			m_VerticesData[i].position = DirectX::SimpleMath::Vector4::Transform(m_VertexPositions[i], transform->GetModel());
			m_VerticesData[i].color = DirectX::SimpleMath::Vector4::One;
		}

		m_VertexBuffer->SetData(m_VerticesData, sizeof(ShapeVertex) * m_VerticesCount);
	}

	~ShapeRenderer() override
	{
		delete[] m_VertexPositions;
		delete[] m_VerticesData;
	}

private:
	int m_VerticesCount;
	DirectX::SimpleMath::Vector4* m_VertexPositions;
	ShapeVertex* m_VerticesData;
};
