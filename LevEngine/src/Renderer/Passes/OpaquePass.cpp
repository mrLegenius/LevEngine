#include "levpch.h"
#include "OpaquePass.h"

#include "Renderer/Material/Material.h"
#include "Renderer/Pipeline/PipelineState.h"
#include "Renderer/Renderer3D.h"
#include "Renderer/RenderParams.h"
#include "Renderer/Shader/ShaderType.h"
#include "Renderer/3D/Mesh.h"
#include "Assets/MaterialAsset.h"
#include "Assets/MeshAsset.h"
#include "Renderer/Camera/SceneCamera.h"
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

        Material* previousMaterial{nullptr};

        // Process static meshes
        const auto staticMeshGroup = registry.group<>(entt::get<Transform, MeshRendererComponent>, entt::exclude<AnimatorComponent>);
        for (const auto entity : staticMeshGroup)
        {
            auto [transform, meshRenderer] = staticMeshGroup.get<Transform, MeshRendererComponent>(entity);

            if (!meshRenderer.enabled) continue;
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

            if (previousMaterial != &material)
                material.Bind(shader);

            Renderer3D::DrawMesh(transform.GetModel(), mesh, shader);

            previousMaterial = &material;
        }

        if (previousMaterial)
		{
            previousMaterial->Unbind(shader);
            previousMaterial = nullptr;
		}

        // Process animated meshes
        const auto animatedMeshGroup = registry.group<>(entt::get<Transform, MeshRendererComponent, AnimatorComponent>);
        for (const auto entity : animatedMeshGroup)
        {
            auto [transform, meshRenderer, animator] = animatedMeshGroup.get<Transform, MeshRendererComponent,
                AnimatorComponent>(entity);

            if (!meshRenderer.enabled) continue;
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

            if (previousMaterial != &material)
			{
                material.Bind(shader);
			}
            
            Renderer3D::DrawMesh(transform.GetModel(), animator.GetFinalBoneMatrices(), mesh, shader);
            
            previousMaterial = &material;
        }

        if (previousMaterial)
        {
            previousMaterial->Unbind(shader);
            previousMaterial = nullptr;
        }
    }

    void OpaquePass::End(entt::registry& registry, RenderParams& params)
    {
        m_PipelineState->Unbind();
    }
}

