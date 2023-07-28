#pragma once
#include <cstdint>
#include <memory>

#include "ClearFlags.h"
#include "D3D11RenderTarget.h"
#include "Texture.h"
#include "RenderPass.h"
namespace LevEngine
{
class ClearPass : public RenderPass
{
public:
	explicit ClearPass(std::shared_ptr<D3D11RenderTarget> renderTarget,
	                   ClearFlags clearFlags = ClearFlags::All,
	                   const Vector4& color = Vector4::Zero,
	                   float depth = 1.0f,
	                   uint8_t stencil = 0);

    explicit ClearPass(std::shared_ptr<Texture> texture,
                       ClearFlags clearFlags = ClearFlags::All,
                       const Vector4& color = Vector4::Zero,
                       float depth = 1.0f,
                       uint8_t stencil = 0);

    ~ClearPass() override = default;

    void Process(RenderParams&) override;

private:
    std::shared_ptr<D3D11RenderTarget> m_RenderTarget;
    std::shared_ptr<Texture> m_Texture;
    ClearFlags m_ClearFlags;
    Vector4 m_ClearColor;
    float m_ClearDepth;
    uint8_t m_ClearStencil;
};
}