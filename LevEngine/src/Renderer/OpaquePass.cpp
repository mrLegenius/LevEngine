#include "levpch.h"
#include "OpaquePass.h"

#include "Material.h"
#include "PipelineState.h"
#include "Renderer3D.h"
#include "RenderParams.h"
#include "3D/Mesh.h"
#include "Assets/MaterialAsset.h"
#include "Assets/MeshAsset.h"
#include "Camera/SceneCamera.h"
#include "Scene/Components/MeshRenderer/MeshRenderer.h"
#include "Scene/Components/Animation/AnimatorComponent.h"
#include "Scene/Components/Transform/Transform.h"

namespace LevEngine
{
    OpaquePass::OpaquePass(const Ref<PipelineState>& pipelineState) : m_PipelineState(pipelineState) { }

    String OpaquePass::PassName() { return "Opaque"; }

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
            auto [transform, meshRenderer] = staticMeshGroup.get<Transform, MeshRendererComponent>(entity);

            if (!meshRenderer.material) continue;
            auto& material = meshRenderer.material->GetMaterial();

            if (material.IsTransparent()) continue;
            if (!meshRenderer.mesh) continue;

            const auto mesh = meshRenderer.mesh->GetMesh();
            if (!mesh) continue;

            if (RenderSettings::UseFrustumCulling)
            {
                if (!mesh->IsOnFrustum(params.Camera->GetFrustum(), transform)) continue;
            }
            
            material.Bind(shader);
            Renderer3D::DrawMesh(transform.GetModel(), mesh, shader);
            material.Unbind(shader);
        }
        
        // Process animated meshes
        const auto animatedMeshGroup = registry.group<>(entt::get<Transform, MeshRendererComponent, AnimatorComponent>);
        for (const auto entity : animatedMeshGroup)
        {
            auto [transform, meshRenderer, animator] = animatedMeshGroup.get<Transform, MeshRendererComponent,
                AnimatorComponent>(entity);

            if (!meshRenderer.material) continue;
            auto& material = meshRenderer.material->GetMaterial();

            if (material.IsTransparent()) continue;
            if (!meshRenderer.mesh) continue;

            const auto mesh = meshRenderer.mesh->GetMesh();
            if (!mesh) continue;

            if (RenderSettings::UseFrustumCulling)
            {
                if (!mesh->IsOnFrustum(params.Camera->GetFrustum(), transform)) continue;
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

