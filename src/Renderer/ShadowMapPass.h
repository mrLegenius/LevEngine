#pragma once
#include "Assets.h"
#include "CascadeShadowMap.h"
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
    ShadowMapPass()
    {
        m_CascadeShadowMap = CreateRef<CascadeShadowMap>(RenderSettings::ShadowMapResolution, RenderSettings::ShadowMapResolution);
        m_ShadowMapConstantBuffer = ConstantBuffer::Create(sizeof ShadowData, 3);
    }

    bool Begin(entt::registry& registry, RenderParams& params) override;
    void Process(entt::registry& registry, RenderParams& params) override;
    void End(entt::registry& registry, RenderParams& params) override;

    void BindConstantBuffer() const
    {
	    m_ShadowMapConstantBuffer->SetData(&m_ShadowData, sizeof ShadowData);
    }

private:
    ShadowData m_ShadowData{};
    Ref<CascadeShadowMap> m_CascadeShadowMap = nullptr;
    Ref<ConstantBuffer> m_ShadowMapConstantBuffer;

    std::vector<Vector4> GetFrustumWorldCorners(const Matrix& view, const Matrix& proj) const;
    Matrix GetCascadeProjection(const Matrix& lightView, std::vector<Vector4> frustrumCorners) const;
};
}
