#pragma once

#include "RasterizerState.h"
#include "RenderTarget.h"
#include "Texture.h"

namespace LevEngine
{
class CascadeShadowMap
{
public:
	CascadeShadowMap(uint32_t width, uint32_t height);

	void Bind(uint32_t slot = 0) const;
	void SetRenderTarget() const;

private:

	Ref<RasterizerState> m_RastState{};
	Ref<Texture> m_Texture{};
	Ref<RenderTarget> m_RenderTarget{};
};
}
