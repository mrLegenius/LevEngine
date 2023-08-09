#pragma once
#include <d3d11.h>
#include <windows.h>

#include "RenderTarget.h"
namespace LevEngine
{
class D3D11Context final
{
public:
	D3D11Context() = default;

	~D3D11Context();
	void Init(uint32_t width, uint32_t height, HWND window);
	void SwapBuffers();
	void ResizeBackBuffer(uint16_t width, uint16_t height);

	Ref<RenderTarget> GetRenderTarget() { return m_RenderTarget; }
private:
	ID3D11Texture2D* m_BackBuffer{};
	Ref<RenderTarget> m_RenderTarget{};
};
}
