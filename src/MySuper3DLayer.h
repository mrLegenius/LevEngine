#pragma once
#include <memory>

#include "D3D11IndexBuffer.h"
#include "Layer.h"

class MySuper3DLayer : public Layer
{
public:
	MySuper3DLayer() : Layer("My Super 3D Layer") { }
	~MySuper3DLayer() override { }

	void OnAttach() override;
	void OnUpdate() override;

private:

	std::unique_ptr<D3D11VertexBuffer> m_VertexBuffer;
	std::unique_ptr<D3D11IndexBuffer> m_IndexBuffer;
	std::unique_ptr<D3D11Shader> m_Shader;
};

