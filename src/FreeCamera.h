#pragma once
#include <SimpleMath.h>

class FreeCamera
{
public:
	FreeCamera() = default;
	

	void Update(float deltaTime);
private:
	void Zoom(float value);
};