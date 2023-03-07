#include "MySuper3DLayer.h"

#include <directxmath.h>
#include "../Kernel/Application.h"
#include "../Renderer/RenderCommand.h"
#include "../Components/QuadRenderer.h"

void MySuper3DLayer::OnAttach()
{
	m_Shader = std::make_unique<D3D11Shader>("./resources/Shaders/MyVeryFirstShader.hlsl");

	QuadVertex vertices[4] = {
		{{0.5f, 0.5f, 0.5f, 1.0f}, { 1.0f, 0.0f, 0.0f, 1.0f}},
		{{-0.5f, -0.5f, 0.5f, 1.0f}, { 0.0f, 0.0f, 1.0f, 1.0f}},
		{{0.5f, -0.5f, 0.5f, 1.0f}, { 0.0f, 1.0f, 0.0f, 1.0f}},
		{{-0.5f, 0.5f, 0.5f, 1.0f}, { 1.0f, 1.0f, 1.0f, 1.0f}},
	};

	Transform transform;
	transform.SetPositionX(0.1f);

	for (auto& vertex : vertices)
		vertex.ApplyModel(transform.GetModel());

	constexpr auto size = std::size(vertices) * sizeof(QuadVertex);
	m_VertexBuffer = std::make_shared<D3D11VertexBuffer>(reinterpret_cast<float*>(vertices), size);

	const BufferLayout layout{
	{ ShaderDataType::Float3, "POSITION" },
	{ ShaderDataType::Float4, "COLOR" },
	};
	m_Shader->SetLayout(layout);
	m_VertexBuffer->SetLayout(layout);

	uint32_t indices[] = { 0,1,2, 1,0,3 };
	m_IndexBuffer = std::make_shared<D3D11IndexBuffer>(indices, std::size(indices));
}

void MySuper3DLayer::OnUpdate(const float deltaTime)
{
	RenderCommand::Begin();
	const auto& window = Application::Get().GetWindow();
	RenderCommand::SetViewport(0, 0, window.GetWidth(), window.GetHeight());

	static float red = 0;
	red += deltaTime;
	if (red >= 1.0f)
		red = 0.0f;

	float color[] = { red, 0.1f, 0.1f, 1.0f};
	RenderCommand::SetClearColor(color);
	RenderCommand::Clear();

	m_Shader->Bind();
	RenderCommand::DrawIndexed(m_VertexBuffer, m_IndexBuffer);

	RenderCommand::End();
}
