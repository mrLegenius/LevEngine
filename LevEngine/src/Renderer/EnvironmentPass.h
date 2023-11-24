#pragma once
#include "RenderPass.h"

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
        const Ref<Texture>& GetEnvironmentCubemap() const { return m_EnvironmentCubemap; }
        const Ref<Texture>& GetEnvironmentIrradianceCubemap() const { return m_EnvironmentIrradianceCubemap; }

    private:
        [[nodiscard]] Ref<Texture> CreateEnvironmentCubemap(const Ref<Texture>& environmentMap) const;
        [[nodiscard]] Ref<Texture> CreateIrradianceCubemap(const Ref<Texture>& environmentCubemap) const;
        [[nodiscard]] Ref<Texture> CreateCubemap(const Ref<Texture>& sourceTexture, uint32_t resolution, const Ref<Shader>& shader) const;

        Ref<Texture> m_EnvironmentMap;
        Ref<Texture> m_EnvironmentCubemap;
        Ref<Texture> m_EnvironmentIrradianceCubemap;

        Ref<Mesh> m_Cube;
    };
}

