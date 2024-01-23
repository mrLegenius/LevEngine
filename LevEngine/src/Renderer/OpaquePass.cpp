#include "levpch.h"
#include "OpaquePass.h"

#include "Material.h"
#include "PipelineState.h"
#include "Renderer3D.h"
#include "RenderParams.h"
#include "Shader.h"
#include "3D/Mesh.h"
#include "Assets/EngineAssets.h"
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
        Material* previousMaterial{nullptr};

        // Process static meshes
        const auto staticShader = ShaderAssets::GBufferVertex();
        staticShader->Bind();
        
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

            //if (previousMaterial != &material)
            //    material.Bind(staticShader);

            material.Bind(staticShader);
            Renderer3D::DrawMesh(transform.GetModel(), mesh, staticShader);
            material.Unbind(staticShader);

            previousMaterial = &material;
        }

        // if (previousMaterial)
        // {
        //     previousMaterial->Unbind(staticShader);
        //     previousMaterial = nullptr;
        // }

        staticShader->Unbind();
        
        const auto animatedShader = ShaderAssets::GBufferAnimatedVertex();
        animatedShader->Bind();
        
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

   //          if (previousMaterial != &material)
			// {
   //              material.Bind(animatedShader);
			// }

            material.Bind(animatedShader);
            Renderer3D::DrawMesh(transform.GetModel(), animator.GetFinalBoneMatrices(), mesh, animatedShader);
            material.Unbind(animatedShader);
            
            previousMaterial = &material;
        }

        // if (previousMaterial)
        // {
        //     previousMaterial->Unbind(animatedShader);
        // }

        animatedShader->Unbind();
    }

    void OpaquePass::End(entt::registry& registry, RenderParams& params)
    {
        m_PipelineState->Unbind();
    }
}

