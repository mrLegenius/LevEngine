#pragma once

#include "Kernel/Core.h"
#include "Renderer/Passes/RenderPass.h"

#include "AtmosphereConstants.h"

namespace LevEngine
{
    class RenderTarget;
    class PipelineState;
    class Shader;
    class ConstantBuffer;
    class Texture;

    class LEV_API EnvironmentPrecomputePass final : public RenderPass
    {
    public:
        explicit EnvironmentPrecomputePass(const Ref<AtmosphereConstants>& atmosphere);

        [[nodiscard]] const Ref<Texture>& GetEnvironmentCubemap() const { return m_EnvironmentCubemap; }

        // The skybox on its own, kept even while an atmosphere is active so the sky pass can composite
        // the two: air in front, stars behind. Null when the scene has no skybox.
        [[nodiscard]] const Ref<Texture>& GetSkyboxCubemap() const { return m_SkyboxCubemap; }
    protected:
        String PassName() override;
        void Process(entt::registry& registry, RenderParams& params) override;
        void End(entt::registry& registry, RenderParams& params) override;

    private:
        Ref<AtmosphereConstants> m_Atmosphere;

        Ref<Texture> m_EnvironmentMap;
        Ref<Texture> m_SkyboxCubemap;
        Ref<Texture> m_EnvironmentCubemap;
        Ref<Texture> m_EnvironmentIrradianceCubemap;
        Ref<Texture> m_EnvironmentPrefilterCubemap;
        Ref<Texture> m_BRDFLutTexture;

        //<--- Procedural sky ---<<
        // Two refresh rates: the sky cubemap is cheap and has to keep up with the sun so the sky
        // never visibly steps, while the irradiance and prefilter maps built from it cost far more
        // and can lag behind by a degree or so without anyone noticing.
        Ref<Texture> m_AtmosphereCubemap;
        GPUAtmosphereData m_SkySnapshot{};
        bool m_HasSky = false;
        bool m_HasSkyLight = false;
        float m_TimeSinceSkyUpdate = 0.0f;
        float m_TimeSinceSkyLightUpdate = 0.0f;

        // Which sky the light maps were built from. The irradiance and prefilter chain reads exactly
        // one thing -- the cubemap above -- so an unchanged cubemap can only produce the identical
        // result, and running it again is pure cost.
        //
        // It used to decide for itself, by testing the atmosphere against its own snapshot with its
        // own thresholds. That let it fire on a camera that had merely changed altitude while the
        // cubemap it would read had not been touched: a full convolution and prefilter, tens of
        // milliseconds, to recompute the same numbers.
        uint32_t m_SkyRevision = 0;
        uint32_t m_SkyLightRevision = 0;

        void ProcessSkybox(entt::registry& registry);
        void ProcessAtmosphere();
        [[nodiscard]] bool NeedsSkyUpdate() const;
        [[nodiscard]] bool NeedsSkyLightUpdate() const;

        [[nodiscard]] static Ref<Texture> CreateEnvironmentCubemap(const Ref<Texture>& environmentMap);
        [[nodiscard]] static Ref<Texture> CreateIrradianceCubemap(const Ref<Texture>& environmentCubemap);
        [[nodiscard]] static Ref<Texture> CreatePrefilterCubemap(const Ref<Texture>& sourceTexture);
        [[nodiscard]] static Ref<Texture> CreateBRDFLutTexture();
        [[nodiscard]] static Ref<Texture> CreateCubemap(const Ref<Texture>& sourceTexture, uint32_t resolution, const Ref<Shader>& shader, bool generateMipMaps);
        [[nodiscard]] static Ref<PipelineState> CreateCubemapPipeline(const Ref<Shader>& shader, const Ref<RenderTarget>& renderTarget);
        static void RenderCube(const Ref<PipelineState>& pipeline, uint32_t resolution);

        // Draw into a cubemap that already exists. The procedural sky is rebuilt over and over, so
        // it cannot allocate a new set of textures every time the sun moves.
        static void RenderCubemap(const Ref<Texture>& destination, uint32_t resolution,
                                  const Ref<Shader>& shader, const Ref<Texture>& sourceTexture);
        static void RenderPrefilterCubemap(const Ref<Texture>& destination, const Ref<Texture>& sourceTexture,
                                           uint32_t resolution);

        static Ref<Texture> CreateRenderTexture(uint32_t resolution, bool generateMipMaps);
        static Array<Matrix, 6> GetCaptureViews();
        static void SetCaptureViewToShader(const Ref<Shader>& shader, const Ref<ConstantBuffer>& constantBuffer);
    };
}
