#pragma once
#include <d3d11_2.h>
#include <dxgi1_3.h>
#include <wrl/client.h>
#include <Renderer/3D/MeshLoading/AnimationLoader.h>

#include "Renderer/RenderContext.h"
#include "Renderer/RenderDevice.h"

namespace LevEngine
{
class D3D11RenderContext final : public RenderContext
{
public:
	D3D11RenderContext() = default;
	~D3D11RenderContext() override = default;
	
	void Init(Ref<RenderDevice> renderDevice, uint32_t width, uint32_t height, bool isVSync, HWND window) override;
	void SwapBuffers(bool isVSync) override;
	void ResizeBackBuffer(uint16_t width, uint16_t height) override;

private:
	Microsoft::WRL::ComPtr<ID3D11DeviceContext2> m_DeviceContext{};
	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_BackBuffer{};
	Microsoft::WRL::ComPtr<IDXGISwapChain2> m_SwapChain{};
};
}
