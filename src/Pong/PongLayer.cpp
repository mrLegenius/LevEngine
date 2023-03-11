#include "PongLayer.h"
#include <iostream>

#include "AIPad.h"
#include "../Kernel/Application.h"
#include "../Renderer/D3D11Shader.h"
#include "../Renderer/RenderCommand.h"
#include "Ball.h"
#include "PlayerPad.h"

static float s_SpeedDelta = 0.01f;

void PongLayer::OnAttach()
{
	auto shader = std::make_shared<D3D11Shader>("./resources/Shaders/MyVeryFirstShader.hlsl");
	shader->SetLayout({
	{ ShaderDataType::Float4, "POSITION" },
	{ ShaderDataType::Float4, "COLOR" },
	});


	auto dotLineShader = std::make_shared<D3D11Shader>("./resources/Shaders/DotLine.hlsl");
	dotLineShader->SetLayout({
	{ ShaderDataType::Float4, "POSITION" },
	{ ShaderDataType::Float4, "COLOR" },
		});

	m_Ball = std::make_shared<Ball>(shader);
	m_Ball->Reset();

	m_LeftPad = std::make_shared<PlayerPad>(-0.8f, shader);
	m_RightPad = std::make_shared<AIPad>(0.8f, shader, m_Ball);

	m_Line = std::make_shared<GameObject>(std::make_shared<QuadRenderer>(dotLineShader));
	m_Line->GetTransform()->SetLocalScale(Vector3(0.02f, 10.0f, 1.0f));
}

void PongLayer::OnUpdate(const float deltaTime)
{
	RenderCommand::Begin();
	const auto& window = Application::Get().GetWindow();
	RenderCommand::SetViewport(0, 0, window.GetWidth(), window.GetHeight());

	float color[] = { 0.1f, 0.1f, 0.1f, 1.0f };
	RenderCommand::SetClearColor(color);
	RenderCommand::Clear();

	static int scoreLeft = 0;
	static int scoreRight = 0;

	static float maxY = 0.95f;
	static float minY = -0.95f;
	static float maxX = 0.95f;
	static float minX = -0.95f;

	m_Ball->Update(deltaTime);

	if (m_Ball->GetTransform()->GetLocalPosition().x > maxX && m_Ball->GetVelocity().x > 0)
	{
		scoreLeft++;
		std::cout << "Score: " << scoreLeft << ":" << scoreRight << std::endl;
		m_Ball->Reset();
	}

	if (m_Ball->GetTransform()->GetLocalPosition().x < minX && m_Ball->GetVelocity().x < 0)
	{
		scoreRight++;
		std::cout << "Score: " << scoreLeft << ":" << scoreRight << std::endl;
		m_Ball->Reset();
	}

	if (m_Ball->GetTransform()->GetLocalPosition().y > maxY && m_Ball->GetVelocity().y > 0
		|| m_Ball->GetTransform()->GetLocalPosition().y < minY && m_Ball->GetVelocity().y < 0)
	{
		m_Ball->ChangeYDirection();
	}

	m_Ball->Draw();

	m_LeftPad->Update(deltaTime);
	m_LeftPad->Draw();

	m_RightPad->Update(deltaTime);
	m_RightPad->Draw();

	if (m_Ball->GetVelocity().x > 0 && m_Ball->Intersects(*m_RightPad))
	{
		m_Ball->ChangeXDirection();
		m_Ball->AddSpeed(s_SpeedDelta);
		m_Ball->AddVelocity(m_RightPad->GetVelocity() / 4.0f);
	}

	if (m_Ball->GetVelocity().x < 0 && m_Ball->Intersects(*m_LeftPad))
	{
		m_Ball->ChangeXDirection();
		m_Ball->AddSpeed(s_SpeedDelta);
		m_Ball->AddVelocity(m_LeftPad->GetVelocity() / 4.0f);
	}

	m_Line->Draw();

	RenderCommand::End();
}