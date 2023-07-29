#pragma once

#include "RasterizerState.h"
#include "RenderTarget.h"
#include "SamplerState.h"
#include "Texture.h"

namespace LevEngine
{
class CascadeShadowMap
{
public:
	CascadeShadowMap(uint32_t width, uint32_t height);

	[[nodiscard]] uint32_t GetWidth() const { return m_Width; }
	[[nodiscard]] uint32_t GetHeight() const { return m_Height; }

	void Bind(uint32_t slot = 0) const;
	void SetRenderTarget() const;

private:

	Ref<RasterizerState> m_RastState{};
	Ref<SamplerState> m_SamplerState{};
	Ref<Texture> m_Texture{};
	Ref<RenderTarget> m_RenderTarget{};

	uint32_t m_Width{};
	uint32_t m_Height{};
};
}
