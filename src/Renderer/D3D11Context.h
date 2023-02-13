#pragma once
#include <inttypes.h>
#include <windows.h>

class D3D11Context
{
public:
	D3D11Context() = default;

	void Init(uint32_t width, uint32_t height, HWND window);
	void SwapBuffers();
private:
};

