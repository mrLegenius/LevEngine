#pragma once
#include <memory>

#include "Transform.h"
#include "../Renderer/D3D11VertexBuffer.h"
#include "../Renderer/D3D11IndexBuffer.h"
#include "../Renderer/D3D11Shader.h"
#include "../Renderer/RenderCommand.h"

class RendererComponent
{
public:
	virtual void Draw(const std::shared_ptr<Transform>& transform)
	{
		Prepare(transform);
		Bind();
		RenderCommand::DrawIndexed(m_VertexBuffer, m_IndexBuffer);
	}
protected:
	explicit RendererComponent(const std::shared_ptr<D3D11Shader>& shader) : m_Shader(shader)
	{

	}

	virtual ~RendererComponent() = default;

	virtual void Bind() const
	{
		m_Shader->Bind();
	}

	virtual void Prepare(const std::shared_ptr<Transform>& transform) = 0;

	std::shared_ptr<D3D11Shader> m_Shader;
	std::shared_ptr<D3D11VertexBuffer> m_VertexBuffer;
	std::shared_ptr<D3D11IndexBuffer> m_IndexBuffer;
};
