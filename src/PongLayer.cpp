#include "PongLayer.h"

#include "Kernel/Application.h"
#include "Renderer/D3D11Shader.h"
#include "Renderer/RenderCommand.h"

struct QuadVertex
{
	float position[4];
	float color[4];
};

class Square
{
public:
	Square(const std::shared_ptr<D3D11Shader> shader) : m_Shader(shader)
	{
		QuadVertex vertices[4] = {
		{{0.5f, 0.5f, 0.5f, 1.0f}, { 1.0f, 1.0f, 1.0f, 1.0f}},
		{{-0.5f, -0.5f, 0.5f, 1.0f}, { 1.0f, 1.0f, 1.0f, 1.0f}},
		{{0.5f, -0.5f, 0.5f, 1.0f}, { 1.0f, 1.0f, 1.0f, 1.0f}},
		{{-0.5f, 0.5f, 0.5f, 1.0f}, { 1.0f, 1.0f, 1.0f, 1.0f}},
		};

		constexpr auto size = std::size(vertices) * sizeof(QuadVertex);
		m_VertexBuffer = std::make_shared<D3D11VertexBuffer>(reinterpret_cast<float*>(vertices), size);
		m_VertexBuffer->SetLayout(m_Shader->GetLayout());

		uint32_t indices[] = { 0,1,2, 1,0,3 };
		m_IndexBuffer = std::make_shared<D3D11IndexBuffer>(indices, std::size(indices));
	}

	void Render() const
	{
		m_Shader->Bind();
		RenderCommand::DrawIndexed(m_VertexBuffer, m_IndexBuffer);
	}

private:
	std::shared_ptr<D3D11Shader> m_Shader;
	std::shared_ptr<D3D11VertexBuffer> m_VertexBuffer;
	std::shared_ptr<D3D11IndexBuffer> m_IndexBuffer;
};

void PongLayer::OnAttach()
{
	auto shader = std::make_shared<D3D11Shader>("./Shaders/MyVeryFirstShader.hlsl");
	shader->SetLayout({
	{ ShaderDataType::Float4, "POSITION" },
	{ ShaderDataType::Float4, "COLOR" },
	});


	m_Square = std::make_shared<Square>(shader);
}

void PongLayer::OnUpdate()
{
	RenderCommand::Begin();
	const auto& window = Application::Get().GetWindow();
	RenderCommand::SetViewport(0, 0, window.GetWidth(), window.GetHeight());

	float color[] = { 0.1f, 0.1f, 0.1f, 1.0f };
	RenderCommand::SetClearColor(color);
	RenderCommand::Clear();

	m_Square->Render();

	RenderCommand::End();
}
