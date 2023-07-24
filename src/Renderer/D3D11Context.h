#pragma once
#include <windows.h>

#include "D3D11RenderTarget.h"
namespace LevEngine
{
class D3D11Context
{
public:
	D3D11Context() = default;
	~D3D11Context();
	void Init(uint32_t width, uint32_t height, HWND window);
	void SwapBuffers();

	std::shared_ptr<D3D11RenderTarget> GetRenderTarget() { return m_RenderTarget; }
private:
	ID3D11Texture2D* m_BackBuffer;
	Ref<D3D11RenderTarget> m_RenderTarget;
};
}
