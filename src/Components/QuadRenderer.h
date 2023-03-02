#pragma once
#include <memory>

#include "RendererComponent.h"
#include "Transform.h"
#include "../Renderer/D3D11VertexBuffer.h"
#include "../Renderer/D3D11IndexBuffer.h"
#include "../Renderer/D3D11Shader.h"

struct QuadVertex
{
	DirectX::SimpleMath::Vector4 position;
	DirectX::SimpleMath::Vector4 color;

	void ApplyModel(DirectX::SimpleMath::Matrix mat)
	{
		position = DirectX::SimpleMath::Vector4::Transform(position, mat);
	}
};

class QuadRenderer : public RendererComponent
{
public:
	QuadRenderer(const std::shared_ptr<D3D11Shader> shader) : RendererComponent(shader)
	{
		constexpr auto size = 4 * sizeof(QuadVertex);
		m_VertexBuffer = std::make_shared<D3D11VertexBuffer>(size);
		m_VertexBuffer->SetLayout(m_Shader->GetLayout());

		uint32_t indices[] = { 0,1,2, 1,0,3 };
		m_IndexBuffer = std::make_shared<D3D11IndexBuffer>(indices, std::size(indices));
	}

	void ApplyTransform(const std::shared_ptr<Transform>& transform) override
	{
		QuadVertex vertices[4] =
		{
			{{0.5f, 0.5f, 0.5f, 1.0f}, { 1.0f, 1.0f, 1.0f, 1.0f}},
			{{-0.5f, -0.5f, 0.5f, 1.0f}, { 1.0f, 1.0f, 1.0f, 1.0f}},
			{{0.5f, -0.5f, 0.5f, 1.0f}, { 1.0f, 1.0f, 1.0f, 1.0f}},
			{{-0.5f, 0.5f, 0.5f, 1.0f}, { 1.0f, 1.0f, 1.0f, 1.0f}},
		};

		for (auto& vertex : vertices)
			vertex.ApplyModel(transform->GetModel());

		m_VertexBuffer->SetData(vertices, sizeof(vertices));
	}
};
