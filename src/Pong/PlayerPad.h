#pragma once
#include "Pad.h"
#include "../Input/Input.h"

class PlayerPad : public Pad
{
public:
	PlayerPad(float xPosition, const std::shared_ptr<D3D11Shader>& shader)
		: Pad(xPosition, shader)
	{
	}

	bool ShouldGoDown() override
	{
		return Input::IsKeyDown(KeyCode::S);
	}
	bool ShouldGoUp() override
	{
		return Input::IsKeyDown(KeyCode::W);
	}
};
