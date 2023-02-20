#include "PongLayer.h"
#include <d3d11.h>

#include "AIPad.h"
#include "SimpleMath.h"
#include "Components/Transform.h"
#include "Kernel/Application.h"
#include "Renderer/D3D11Shader.h"
#include "Renderer/RenderCommand.h"
#include "Ball.h"
#include "PlayerPad.h"

static DirectX::SimpleMath::Vector3 s_StartVelocity = { 0.5f, 0.25f, 0.0f };
static float s_SpeedDelta = 0.01f;

void PongLayer::OnAttach()
{
	auto shader = std::make_shared<D3D11Shader>("./Shaders/MyVeryFirstShader.hlsl");
	shader->SetLayout({
	{ ShaderDataType::Float4, "POSITION" },
	{ ShaderDataType::Float4, "COLOR" },
	});

	m_Ball = std::make_shared<Ball>(shader);
	m_Ball->SetVelocity(s_StartVelocity);

	m_LeftPad = std::make_shared<PlayerPad>(-0.8f, shader);
	m_RightPad = std::make_shared<AIPad>(0.8f, shader, m_Ball);
}

void PongLayer::OnUpdate()
{
	RenderCommand::Begin();
	const auto& window = Application::Get().GetWindow();
	RenderCommand::SetViewport(0, 0, window.GetWidth(), window.GetHeight());

	float color[] = { 0.1f, 0.1f, 0.1f, 1.0f };
	RenderCommand::SetClearColor(color);
	RenderCommand::Clear();

	static float maxY = 0.95f;
	static float minY = -0.95f;
	static float maxX = 0.95f;
	static float minX = -0.95f;

	constexpr float deltaTime = 1.0f / 144.0f;
	m_Ball->Update(deltaTime);

	if (m_Ball->GetTransform()->position.x > maxX && m_Ball->GetVelocity().x > 0
		|| m_Ball->GetTransform()->position.x < minX && m_Ball->GetVelocity().x < 0)
	{
		m_Ball->ChangeXDirection();
		m_Ball->AddSpeed(s_SpeedDelta);
	}

	if (m_Ball->GetTransform()->position.y > maxY && m_Ball->GetVelocity().y > 0
		|| m_Ball->GetTransform()->position.y < minY && m_Ball->GetVelocity().y < 0)
	{
		m_Ball->ChangeYDirection();
	}

	m_Ball->Draw();

	m_LeftPad->Update(deltaTime);
	m_LeftPad->Draw();

	m_RightPad->Update(deltaTime);
	m_RightPad->Draw();

	RenderCommand::End();
}