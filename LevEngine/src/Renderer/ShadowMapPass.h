#pragma once
#include "RenderPass.h"
#include "RenderSettings.h"

#include "Math/Matrix.h"

namespace LevEngine
{
    class ConstantBuffer;
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

        String PassName() override;
        bool Begin(entt::registry& registry, RenderParams& params) override;
        void Process(entt::registry& registry, RenderParams& params) override;
        void End(entt::registry& registry, RenderParams& params) override;

    private:
        ShadowData m_ShadowData{};
        Ref<CascadeShadowMap> m_CascadeShadowMap = nullptr;
        Ref<ConstantBuffer> m_ShadowMapConstantBuffer;

        [[nodiscard]] static Vector<Vector4> GetFrustumWorldCorners(const Matrix& view, const Matrix& proj);
        [[nodiscard]] static Matrix GetCascadeProjection(const Matrix& lightView, Vector<Vector4> frustumCorners);
    };
}
