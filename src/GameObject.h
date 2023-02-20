#pragma once
#include "Components/QuadRenderer.h"
#include "Renderer/RenderCommand.h"

inline void DrawQuad(const std::shared_ptr<Transform>& transform, const std::shared_ptr<QuadRenderer>& quad)
{
	quad->ApplyTransform(transform);
	quad->Bind();
	RenderCommand::DrawIndexed(quad->GetVertexBuffer(), quad->GetIndexBuffer());
}

class GameObject
{
public:
	std::shared_ptr<Transform>& GetTransform() { return m_Transform; }
	std::shared_ptr<QuadRenderer>& GetQuad() { return m_QuadRenderer; }

	void Draw() const
	{
		DrawQuad(m_Transform, m_QuadRenderer);
	}
protected:
	explicit GameObject(const std::shared_ptr<QuadRenderer> quadRenderer)
		: m_Transform(std::make_shared<Transform>()),
		m_QuadRenderer(quadRenderer) { }

	std::shared_ptr<Transform> m_Transform;
	std::shared_ptr<QuadRenderer> m_QuadRenderer;
};