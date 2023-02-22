#pragma once
#include "../Components/QuadRenderer.h"
#include "../Renderer/RenderCommand.h"
#include "DirectXCollision.h"

inline void DrawQuad(const std::shared_ptr<Transform>& transform, const std::shared_ptr<QuadRenderer>& quad)
{
	quad->ApplyTransform(transform);
	quad->Bind();
	RenderCommand::DrawIndexed(quad->GetVertexBuffer(), quad->GetIndexBuffer());
}

class GameObject
{
public:
	virtual ~GameObject() = default;
	std::shared_ptr<Transform>& GetTransform() { return m_Transform; }
	std::shared_ptr<QuadRenderer>& GetQuad() { return m_QuadRenderer; }
	bool Intersects(const GameObject& go) const { return m_BoundingBox->Intersects(*go.m_BoundingBox); }

	virtual void Update()
	{
		m_DefaultBoundingBox.Transform(*m_BoundingBox, m_Transform->GetModel());
	}

	void Draw() const
	{
		DrawQuad(m_Transform, m_QuadRenderer);
	}

protected:
	explicit GameObject(const std::shared_ptr<QuadRenderer> quadRenderer)
		: m_Transform(std::make_shared<Transform>()),
		m_QuadRenderer(quadRenderer)
	{
		m_BoundingBox = std::make_shared<DirectX::BoundingBox>(m_DefaultBoundingBox.Center, m_DefaultBoundingBox.Extents);
	}

	DirectX::BoundingBox m_DefaultBoundingBox = DirectX::BoundingBox(DirectX::SimpleMath::Vector3::Zero, DirectX::SimpleMath::Vector3::One / 2.0f);

	std::shared_ptr<Transform> m_Transform;
	std::shared_ptr<QuadRenderer> m_QuadRenderer;
	std::shared_ptr<DirectX::BoundingBox> m_BoundingBox;
};