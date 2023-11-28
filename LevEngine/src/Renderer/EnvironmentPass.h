#pragma once
#include "RenderPass.h"

#include "DataTypes/Array.h"
#include "Math/Matrix.h"

namespace LevEngine
{
    class ConstantBuffer;
    class Texture;
    class Mesh;
    class Shader;

    class EnvironmentPass : public RenderPass
    {
    public:
        EnvironmentPass();
        String PassName() override;
        void Process(entt::registry& registry, RenderParams& params) override;
        void End(entt::registry& registry, RenderParams& params) override;
        const Ref<Texture>& GetEnvironmentCubemap() const { return m_EnvironmentCubemap; }
        const Ref<Texture>& GetEnvironmentIrradianceCubemap() const { return m_EnvironmentIrradianceCubemap; }
        const Ref<Texture>& GetEnvironmentPreFilterCubemap() const { return m_EnvironmentPrefilterCubemap; }

    private:
        [[nodiscard]] Ref<Texture> CreateEnvironmentCubemap(const Ref<Texture>& environmentMap) const;
        [[nodiscard]] Ref<Texture> CreateIrradianceCubemap(const Ref<Texture>& environmentCubemap) const;
        Ref<Texture> CreatePrefilterCubemap(const Ref<Texture>& sourceTexture) const;
        Ref<Texture> CreateBRDFLutTexture() const;
        [[nodiscard]] Ref<Texture> CreateCubemap(const Ref<Texture>& sourceTexture, uint32_t resolution, const Ref<Shader>& shader, bool generateMipMaps) const;
        static Ref<Texture> CreateRenderTexture(const uint32_t resolution, bool generateMipMaps);
        static Array<Matrix, 6> GetCaptureViews();
        static void SetCaptureViewToShader(const Ref<Shader>& shader, const Ref<ConstantBuffer>& constantBuffer);

        Ref<Texture> m_EnvironmentMap;
        Ref<Texture> m_EnvironmentCubemap;
        Ref<Texture> m_EnvironmentIrradianceCubemap;
        Ref<Texture> m_EnvironmentPrefilterCubemap;
        Ref<Texture> m_BRDFLutTexture;

        Ref<Mesh> m_Cube;
    };
}

