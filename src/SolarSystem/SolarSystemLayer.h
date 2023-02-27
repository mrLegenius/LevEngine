#pragma once
#include "../Kernel/Layer.h"

class SolarSystemLayer : public Layer
{
public:
	SolarSystemLayer() : Layer("Solar System Layer") { }
	~SolarSystemLayer() override = default;

	void OnAttach() override;
	void OnUpdate() override;
};

