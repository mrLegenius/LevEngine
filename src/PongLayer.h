#pragma once
#include "Kernel/Layer.h"

class Square;
class PongLayer final : public Layer
{
public:
	PongLayer() : Layer("Pong Layer") { }
	~PongLayer() override = default;

	void OnAttach() override;
	void OnUpdate() override;

private:
	std::shared_ptr<Square> m_Square;
};

