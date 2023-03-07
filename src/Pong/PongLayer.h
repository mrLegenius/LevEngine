#pragma once
#include "../GameObject.h"
#include "../Kernel/Layer.h"

class Pad;
class Ball;

class PongLayer final : public Layer
{
public:
	PongLayer() : Layer("Pong Layer") { }
	~PongLayer() override = default;

	void OnAttach() override;
	void OnUpdate(float deltaTime) override;

private:
	std::shared_ptr<Ball> m_Ball;
	std::shared_ptr<Pad> m_LeftPad;
	std::shared_ptr<Pad> m_RightPad;
	std::shared_ptr<GameObject> m_Line;
};

