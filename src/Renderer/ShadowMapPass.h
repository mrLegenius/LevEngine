#pragma once
#include "Assets.h"
#include "D3D11CascadeShadowMap.h"
#include "Renderer3D.h"
#include "RenderParams.h"
#include "RenderPass.h"
#include "RenderSettings.h"

namespace LevEngine
{
class ShadowMapPass : public RenderPass
{
    struct ShadowData
    {
        alignas(16) Matrix ViewProjection[RenderSettings::CascadeCount];
        alignas(16) float Distances[RenderSettings::CascadeCount];
        alignas(16) float ShadowMapDimensions;
    };
public:
    explicit ShadowMapPass(entt::registry& registry)
        : m_Registry(registry)
    {
        m_CascadeShadowMap = CreateRef<D3D11CascadeShadowMap>(RenderSettings::ShadowMapResolution, RenderSettings::ShadowMapResolution);
        m_ShadowMapConstantBuffer = ConstantBuffer::Create(sizeof ShadowData, 3);
    }

    bool Begin(RenderParams& params) override;
    void Process(RenderParams& params) override;
    void End(RenderParams& params) override;

    void BindConstantBuffer() const
    {
	    m_ShadowMapConstantBuffer->SetData(&m_ShadowData, sizeof ShadowData);
    }

private:
    entt::registry& m_Registry;
    ShadowData m_ShadowData{};
    Ref<D3D11CascadeShadowMap> m_CascadeShadowMap = nullptr;
    Ref<ConstantBuffer> m_ShadowMapConstantBuffer;

    std::vector<Vector4> GetFrustumWorldCorners(const Matrix& view, const Matrix& proj) const;
    Matrix GetCascadeProjection(const Matrix& lightView, std::vector<Vector4> frustrumCorners) const;
};
}
