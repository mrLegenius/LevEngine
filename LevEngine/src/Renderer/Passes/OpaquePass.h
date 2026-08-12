#pragma once

#include "Kernel/Core.h"
#include "RenderPass.h"
#include "Renderer/3D/MeshBatcher.h"

namespace LevEngine
{
    class PipelineState;
    class Shader;

    class LEV_API OpaquePass final : public RenderPass
    {
    public:
        // instancedShader is the WITH_INSTANCING variant of the pipeline's shader. Passing null
        // keeps the pass on the one-draw-call-per-mesh path.
        explicit OpaquePass(const Ref<PipelineState>& pipelineState, const Ref<Shader>& instancedShader = nullptr);

        String PassName() override;
        bool Begin(entt::registry& registry, RenderParams& params) override;
        void Process(entt::registry& registry, RenderParams& params) override;
        void End(entt::registry& registry, RenderParams& params) override;

    private:
        void ProcessStaticMeshes(entt::registry& registry, const RenderParams& params, const Ref<Shader>& shader) const;
        void ProcessStaticMeshesInstanced(entt::registry& registry, const RenderParams& params);
        static void ProcessAnimatedMeshes(entt::registry& registry, const RenderParams& params);

        Ref<PipelineState> m_PipelineState;
        Ref<Shader> m_InstancedShader;
        MeshBatcher m_Batcher;
    };
}
