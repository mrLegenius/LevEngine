#include "pch.h"
#include "D3D11ForwardTechnique.h"

#include "Kernel/Application.h"

D3D11ForwardTechnique::D3D11ForwardTechnique()
{
	const auto mainRenderTarget = Application::Get().GetWindow().GetContext()->GetRenderTarget();

    m_DepthOnlyRenderTarget = CreateRef<D3D11RenderTarget>();
    m_DepthOnlyRenderTarget->AttachTexture(AttachmentPoint::DepthStencil, mainRenderTarget->GetTexture(AttachmentPoint::DepthStencil));

    m_GeometryPipeline.SetRenderTarget(mainRenderTarget);
    m_GeometryPipeline.GetRasterizerState().SetCullMode(CullMode::None);

    //Skybox
    {
        m_SkyboxPipeline.SetRenderTarget(mainRenderTarget);

        DepthMode depthMode{ true, DepthWrite::Enable, CompareFunction::LessOrEqual };
        m_SkyboxPipeline.GetRasterizerState().SetCullMode(CullMode::None);
        m_SkyboxPipeline.GetRasterizerState().SetDepthClipEnabled(false);
        m_SkyboxPipeline.GetDepthStencilState().SetDepthMode(depthMode);
    }
}

D3D11ForwardTechnique::~D3D11ForwardTechnique()
{
}

void D3D11ForwardTechnique::StartOpaquePass()
{
    m_GeometryPipeline.GetRenderTarget()->Clear();
    m_GeometryPipeline.Bind();
}

void D3D11ForwardTechnique::StartSkyboxPass()
{
    m_GeometryPipeline.Unbind();
    m_SkyboxPipeline.Bind();
}

void D3D11ForwardTechnique::End()
{
    m_GeometryPipeline.Unbind();
}
