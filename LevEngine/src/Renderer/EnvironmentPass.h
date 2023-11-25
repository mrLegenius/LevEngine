#pragma once
#include "ConstantBuffer.h"
#include "RenderPass.h"
#include "DataTypes/Array.h"
#include "Kernel/Application.h"

namespace LevEngine
{
    class Texture;
    class Mesh;
    class Shader;

    class EnvironmentPass : public RenderPass
    {
    public:
        EnvironmentPass();
        void Process(entt::registry& registry, RenderParams& params) override;
        void End(entt::registry& registry, RenderParams& params) override;
        const Ref<Texture>& GetEnvironmentCubemap() const { return m_EnvironmentCubemap; }
        const Ref<Texture>& GetEnvironmentIrradianceCubemap() const { return m_EnvironmentIrradianceCubemap; }
        const Ref<Texture>& GetEnvironmentPreFilterCubemap() const { return m_EnvironmentPrefilterCubemap; }

    private:
        [[nodiscard]] Ref<Texture> CreateEnvironmentCubemap(const Ref<Texture>& environmentMap) const;
        [[nodiscard]] Ref<Texture> CreateIrradianceCubemap(const Ref<Texture>& environmentCubemap) const;
        Ref<Texture> CreatePrefilterCubemap(const Ref<Texture>& sourceTexture) const;
        [[nodiscard]] Ref<Texture> CreateCubemap(const Ref<Texture>& sourceTexture, uint32_t resolution, const Ref<Shader>& shader) const;
        static Ref<Texture> CreateRenderTexture(const uint32_t resolution, bool generateMipMaps);
        static Array<Matrix, 6> GetCaptureViews();
        static void SetCaptureViewToShader(const Ref<Shader>& shader, const Ref<ConstantBuffer>& constantBuffer);

        Ref<Texture> m_EnvironmentMap;
        Ref<Texture> m_EnvironmentCubemap;
        Ref<Texture> m_EnvironmentIrradianceCubemap;
        Ref<Texture> m_EnvironmentPrefilterCubemap;

        Ref<Mesh> m_Cube;
    };
}

