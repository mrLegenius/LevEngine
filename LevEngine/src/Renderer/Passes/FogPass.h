#pragma once

#include "Kernel/Core.h"
#include "RenderPass.h"

namespace LevEngine
{
    class ConstantBuffer;
    class PipelineState;
    class RenderTarget;
    class Texture;

    // Blends exponential height fog over the lit scene, using the depth buffer to reconstruct
    // world positions. Runs on the linear HDR target before post processing.
    class LEV_API FogPass final : public RenderPass
    {
    public:
        FogPass(const Ref<RenderTarget>& renderTarget, const Ref<Texture>& depthTexture);

        String PassName() override;
        bool Begin(entt::registry& registry, RenderParams& params) override;
        void Process(entt::registry& registry, RenderParams& params) override;
        void End(entt::registry& registry, RenderParams& params) override;
        void SetViewport(Viewport viewport) override;

    private:
        struct alignas(16) GPUFogData
        {
            Matrix InverseViewProjection;

            Vector3 CameraPosition;
            float Density;

            Vector3 Color;
            float HeightFalloff;

            float Height;
            float StartDistance;
            float MaxOpacity;
            float AffectsSkybox;

            Vector3 SunDirection;
            float SunScatteringIntensity;

            Vector3 SunColor;
            float SunScatteringExponent;
        };

        static constexpr uint32_t FogConstantBufferSlot = 9;

        Ref<PipelineState> m_Pipeline;
        Ref<Texture> m_DepthTexture;
        Ref<ConstantBuffer> m_ConstantBuffer;
    };
}
