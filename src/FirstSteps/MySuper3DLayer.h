#pragma once
#include <memory>

#include "../Kernel/Layer.h"
#include "../Renderer/D3D11IndexBuffer.h"
#include "../Renderer/D3D11VertexBuffer.h"

using namespace LevEngine;

class MySuper3DLayer : public Layer
{
public:
	MySuper3DLayer() : Layer("My Super 3D Layer") { }
	~MySuper3DLayer() override { }

	void OnAttach() override;
	void OnUpdate(float deltaTime) override;

private:

	std::shared_ptr<D3D11VertexBuffer> m_VertexBuffer;
	std::shared_ptr<D3D11IndexBuffer> m_IndexBuffer;
	std::unique_ptr<D3D11Shader> m_Shader;
};

