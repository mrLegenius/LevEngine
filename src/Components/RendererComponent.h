#pragma once
#include <memory>

#include "Transform.h"
#include "../Renderer/VertexBuffer.h"
#include "../Renderer/IndexBuffer.h"
#include "../Renderer/Shader.h"
#include "../Renderer/RenderCommand.h"

class RendererComponent
{
public:
	virtual void Draw(const Transform& transform)
	{
		Prepare(transform);
		Bind();
		RenderCommand::DrawIndexed(m_VertexBuffer, m_IndexBuffer);
	}
protected:
	explicit RendererComponent(const Ref<Shader>& shader) : m_Shader(shader)
	{

	}

	virtual ~RendererComponent() = default;

	virtual void Bind() const
	{
		m_Shader->Bind();
	}

	virtual void Prepare(const Transform& transform) = 0;

	Ref<Shader> m_Shader;
	Ref<VertexBuffer> m_VertexBuffer;
	Ref<IndexBuffer> m_IndexBuffer;
};
