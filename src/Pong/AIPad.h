#pragma once
#include "Ball.h"
#include "Pad.h"

class AIPad : public Pad
{
public:
	AIPad(float xPosition, const std::shared_ptr<D3D11Shader>& shader, const std::shared_ptr<Ball> ball)
		: Pad(xPosition, shader), m_Ball(ball)
	{

	}

	bool ShouldGoDown() override
	{
		return m_Ball->GetTransform()->GetLocalPosition().y < GetTransform()->GetLocalPosition().y;
	}
	bool ShouldGoUp() override
	{
		return m_Ball->GetTransform()->GetLocalPosition().y > GetTransform()->GetLocalPosition().y;
	}

private:
	std::shared_ptr<Ball> m_Ball;
};
