#pragma once
#include "RenderPass.h"
#include "RenderSettings.h"
#include "DataTypes/Vector.h"


namespace LevEngine
{
    class CascadeShadowMap;

    class ShadowMapPass : public RenderPass
{
    struct ShadowData
    {
        alignas(16) Matrix ViewProjection[RenderSettings::CascadeCount];
        alignas(16) float Distances[RenderSettings::CascadeCount];
        alignas(16) float ShadowMapDimensions;
    };
public:
    ShadowMapPass();

    bool Begin(entt::registry& registry, RenderParams& params) override;
    void Process(entt::registry& registry, RenderParams& params) override;
    void End(entt::registry& registry, RenderParams& params) override;

private:
    ShadowData m_ShadowData{};
    Ref<CascadeShadowMap> m_CascadeShadowMap = nullptr;
    Ref<ConstantBuffer> m_ShadowMapConstantBuffer;

    [[nodiscard]] Vector<Vector4> GetFrustumWorldCorners(const Matrix& view, const Matrix& proj) const;
    [[nodiscard]] Matrix GetCascadeProjection(const Matrix& lightView, Vector<Vector4> frustumCorners) const;
};
}
