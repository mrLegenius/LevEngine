#pragma once
#include "DataTypes/Array.h"
#include "Math/Matrix.h"

#include "Renderer/RenderPass.h"

namespace LevEngine
{
    class RenderTarget;
    class PipelineState;
    class Shader;
    class ConstantBuffer;
    class Texture;

    class EnvironmentPrecomputePass final : public RenderPass
    {
    public:
        [[nodiscard]] const Ref<Texture>& GetEnvironmentCubemap() const { return m_EnvironmentCubemap; }
    protected:
        String PassName() override;
        void Process(entt::registry& registry, RenderParams& params) override;
        void End(entt::registry& registry, RenderParams& params) override;

    private:
        Ref<Texture> m_EnvironmentMap;
        Ref<Texture> m_EnvironmentCubemap;
        Ref<Texture> m_EnvironmentIrradianceCubemap;
        Ref<Texture> m_EnvironmentPrefilterCubemap;
        Ref<Texture> m_BRDFLutTexture;
        
        [[nodiscard]] static Ref<Texture> CreateEnvironmentCubemap(const Ref<Texture>& environmentMap);
        [[nodiscard]] static Ref<Texture> CreateIrradianceCubemap(const Ref<Texture>& environmentCubemap);
        [[nodiscard]] static Ref<Texture> CreatePrefilterCubemap(const Ref<Texture>& sourceTexture);
        [[nodiscard]] static Ref<Texture> CreateBRDFLutTexture();
        [[nodiscard]] static Ref<Texture> CreateCubemap(const Ref<Texture>& sourceTexture, uint32_t resolution, const Ref<Shader>& shader, bool generateMipMaps);
        [[nodiscard]] static Ref<PipelineState> CreateCubemapPipeline(const Ref<Shader>& shader, const Ref<RenderTarget>& renderTarget);
        static void RenderCube(const Ref<PipelineState>& pipeline, uint32_t resolution);

        static Ref<Texture> CreateRenderTexture(uint32_t resolution, bool generateMipMaps);
        static Array<Matrix, 6> GetCaptureViews();
        static void SetCaptureViewToShader(const Ref<Shader>& shader, const Ref<ConstantBuffer>& constantBuffer);
    };
}
