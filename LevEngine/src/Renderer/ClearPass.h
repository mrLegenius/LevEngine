#pragma once

#include "ClearFlags.h"
#include "RenderPass.h"
#include "entt/entt.hpp"

namespace LevEngine
{
	class Texture;
	class RenderTarget;

	class ClearPass : public RenderPass
{
public:
	explicit ClearPass(const Ref<RenderTarget>& renderTarget,
	                   ClearFlags clearFlags = ClearFlags::All,
	                   const Vector4& color = Vector4::Zero,
	                   float depth = 1.0f,
	                   uint8_t stencil = 0);

    explicit ClearPass(const Ref<Texture>& texture,
                       ClearFlags clearFlags = ClearFlags::All,
                       const Vector4& color = Vector4::Zero,
                       float depth = 1.0f,
                       uint8_t stencil = 0);

    ~ClearPass() override = default;

    void Process(entt::registry&, RenderParams&) override;

private:
    Ref<RenderTarget> m_RenderTarget;
    Ref<Texture> m_Texture;
    ClearFlags m_ClearFlags;
    Vector4 m_ClearColor;
    float m_ClearDepth;
    uint8_t m_ClearStencil;
};
}
