#pragma once
#include <d3d11.h>

#include "Renderer/RenderContext.h"

namespace LevEngine
{
class D3D11RendererContext final : public RenderContext
{
public:
	D3D11RendererContext() = default;

	~D3D11RendererContext() override;
	void Init(uint32_t width, uint32_t height, HWND window) override;
	void SwapBuffers() override;
	void ResizeBackBuffer(uint16_t width, uint16_t height) override;

private:
	ID3D11Texture2D* m_BackBuffer{};
};
}
