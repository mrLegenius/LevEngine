#pragma once

#include "Kernel/Core.h"
#include "RenderPass.h"
#include "Renderer/RenderSettings.h"
#include "Renderer/3D/MeshBatcher.h"

namespace LevEngine
{
    class ConstantBuffer;
    class PlanetChunk;
    class CascadeShadowMap;
    class Shader;

    class LEV_API ShadowMapPass : public RenderPass
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
        void ProcessStaticMeshes(entt::registry& registry);

        // Planet chunks are not entities and carry no MeshRendererComponent, so they are walked
        // separately. They always cast: a planet is the largest shadow caster in any scene it is in,
        // and a mountain that does not shade its own valley reads as flat.
        void ProcessPlanets(entt::registry& registry, const RenderParams& params);
        void ProcessStaticMeshesInstanced(entt::registry& registry);
        void ProcessAnimatedMeshes(entt::registry& registry);
        void BindShadowData(const Ref<Shader>& shader) const;

        ShadowData m_ShadowData{};
        Ref<CascadeShadowMap> m_CascadeShadowMap = nullptr;
        Ref<ConstantBuffer> m_ShadowMapConstantBuffer;
        MeshBatcher m_Batcher;

        //<--- Kept across frames so collecting the casters does not allocate every frame ---<<
        Vector<PlanetChunk*> m_PlanetShadowCasters;

        [[nodiscard]] static Vector<Vector4> GetFrustumWorldCorners(const Matrix& view, const Matrix& proj);
        [[nodiscard]] static Matrix GetCascadeProjection(const Matrix& lightView, Vector<Vector4> frustumCorners);
    };
}
