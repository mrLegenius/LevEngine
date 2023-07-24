#pragma once
#include <d3d11.h>
#include <string>

#include "RenderSettings.h"
namespace LevEngine
{
class D3D11CascadeShadowMap
{
public:
	D3D11CascadeShadowMap(uint32_t width, uint32_t height);
	~D3D11CascadeShadowMap();

	[[nodiscard]] uint32_t GetWidth() const { return m_Width; }
	[[nodiscard]] uint32_t GetHeight() const { return m_Height; }

	void Bind(uint32_t slot = 0) const;
	void SetRenderTarget() const;

private:
	ID3D11Texture2D* m_Texture{};
	ID3D11ShaderResourceView* m_ShaderResourceView{};
	ID3D11DepthStencilView* m_DepthStencilView{};
	ID3D11SamplerState* m_SamplerState{};
	ID3D11RasterizerState* m_RastState{};

	uint32_t m_Width{};
	uint32_t m_Height{};
};
}
