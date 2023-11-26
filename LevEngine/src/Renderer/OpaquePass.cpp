#include "levpch.h"
#include "OpaquePass.h"

#include "Renderer3D.h"
#include "3D/Mesh.h"
#include "Scene/Components/Animation/AnimatorComponent.h"
#include "Scene/Components/Transform/Transform.h"

namespace LevEngine
{
    OpaquePass::OpaquePass(const Ref<PipelineState>& pipelineState) : m_PipelineState(pipelineState) { }

    bool OpaquePass::Begin(entt::registry& registry, RenderParams& params)
    {
        m_PipelineState->Bind();
        
        return RenderPass::Begin(registry, params);
    }

    void OpaquePass::Process(entt::registry& registry, RenderParams& params)
    {
        const auto shader = m_PipelineState->GetShader(ShaderType::Vertex);

        // Process static meshes
        const auto staticMeshGroup = registry.group<>(entt::get<Transform, MeshRendererComponent>, entt::exclude<AnimatorComponent>);
        for (const auto entity : staticMeshGroup)
        {
            Transform transform = staticMeshGroup.get<Transform>(entity);
            const MeshRendererComponent meshRenderer = staticMeshGroup.get<MeshRendererComponent>(entity);

            if (!meshRenderer.material) continue;
            auto& material = meshRenderer.material->GetMaterial();

            if (material.IsTransparent()) continue;
            if (!meshRenderer.mesh) continue;

            const auto mesh = meshRenderer.mesh->GetMesh();
            if (!mesh) continue;

            if (RenderSettings::UseFrustumCulling)
            {
                if (!mesh->IsOnFrustum(params.Camera.GetFrustum(), transform)) continue;
            }
            
            material.Bind(shader);
            Renderer3D::DrawMesh(transform.GetModel(), mesh, shader);
            material.Unbind(shader);
        }
        
        // Process animated meshes
        const auto animatedMeshGroup = registry.group<>(entt::get<Transform, MeshRendererComponent, AnimatorComponent>);
        for (const auto entity : animatedMeshGroup)
        {
            Transform transform = animatedMeshGroup.get<Transform>(entity);
            const MeshRendererComponent meshRenderer = animatedMeshGroup.get<MeshRendererComponent>(entity);
            const AnimatorComponent animator = animatedMeshGroup.get<AnimatorComponent>(entity);

            if (!meshRenderer.material) continue;
            auto& material = meshRenderer.material->GetMaterial();

            if (material.IsTransparent()) continue;
            if (!meshRenderer.mesh) continue;

            const auto mesh = meshRenderer.mesh->GetMesh();
            if (!mesh) continue;

            if (RenderSettings::UseFrustumCulling)
            {
                if (!mesh->IsOnFrustum(params.Camera.GetFrustum(), transform)) continue;
            }
            
            material.Bind(shader);
            Renderer3D::DrawMesh(transform.GetModel(), animator.GetFinalBoneMatrices(), mesh, shader);
            material.Unbind(shader);
        }
    }

    void OpaquePass::End(entt::registry& registry, RenderParams& params)
    {
        m_PipelineState->Unbind();
    }
}

