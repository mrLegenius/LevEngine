#include "levpch.h"
#include "OpaquePass.h"

#include "Assets/EngineAssets.h"
#include "Renderer/Material/Material.h"
#include "Renderer/Pipeline/PipelineState.h"
#include "Renderer/Renderer3D.h"
#include "Renderer/RenderParams.h"
#include "Renderer/Shader/ShaderType.h"
#include "Renderer/3D/Mesh.h"
#include "Assets/MaterialAsset.h"
#include "Assets/MeshAsset.h"
#include "Renderer/RenderSettings.h"
#include "Renderer/Camera/SceneCamera.h"
#include "Renderer/Shader/Shader.h"
#include "Scene/Components/MeshRenderer/MeshRenderer.h"
#include "Scene/Components/Animation/AnimatorComponent.h"
#include "Scene/Components/Transform/Transform.h"

namespace LevEngine
{
    OpaquePass::OpaquePass(const Ref<PipelineState>& pipelineState, const Ref<Shader>& instancedShader)
        : m_PipelineState(pipelineState), m_InstancedShader(instancedShader) { }

    String OpaquePass::PassName() { return "Opaque"; }

    bool OpaquePass::Begin(entt::registry& registry, RenderParams& params)
    {
        m_PipelineState->Bind();

        return RenderPass::Begin(registry, params);
    }

    void OpaquePass::Process(entt::registry& registry, RenderParams& params)
    {
        LEV_PROFILE_FUNCTION();

        const auto& shader = m_PipelineState->GetShader(ShaderType::Vertex);

        if (RenderSettings::UseInstancing && m_InstancedShader)
            ProcessStaticMeshesInstanced(registry, params);
        else
            ProcessStaticMeshes(registry, params, shader);

        ProcessAnimatedMeshes(registry, params);
    }

    void OpaquePass::ProcessStaticMeshes(entt::registry& registry, const RenderParams& params,
                                         const Ref<Shader>& shader) const
    {
        LEV_PROFILE_FUNCTION();

        Material* previousMaterial{nullptr};

        const auto staticMeshGroup = registry.group<>(entt::get<Transform, MeshRendererComponent>, entt::exclude<AnimatorComponent>);
        for (const auto entity : staticMeshGroup)
        {
            auto [transform, meshRenderer] = staticMeshGroup.get<Transform, MeshRendererComponent>(entity);

            if (!meshRenderer.enabled) continue;

            auto& material = meshRenderer.material ? meshRenderer.material->GetMaterial() : *Renderer3D::MissingMaterial;
            if (material.IsTransparent()) continue;

            if (!meshRenderer.mesh) continue;

            Queue<Ref<Mesh>> meshesToRender;
            const auto initialMesh = meshRenderer.mesh->GetMesh();

            if (!initialMesh) continue;

            if (previousMaterial != &material)
                material.Bind(shader);

            meshesToRender.push(initialMesh);

            while (meshesToRender.size() > 0)
            {
                auto mesh = meshesToRender.front();
                meshesToRender.pop();

                for (auto subMesh : mesh->GetSubMeshes())
                {
                    if (subMesh)
                        meshesToRender.push(subMesh);
                }

                if (RenderSettings::UseFrustumCulling)
                    if (!mesh->IsOnFrustum(params.Camera->GetFrustum(), transform)) continue;

                if (mesh->IndexBuffer)
                    Renderer3D::DrawMesh(transform.GetModel(), mesh, shader);
            }

            previousMaterial = &material;
        }

        if (previousMaterial)
            previousMaterial->Unbind(shader);
    }

    void OpaquePass::ProcessStaticMeshesInstanced(entt::registry& registry, const RenderParams& params)
    {
        LEV_PROFILE_FUNCTION();

        m_Batcher.Clear();

        const Frustum* frustum = RenderSettings::UseFrustumCulling ? &params.Camera->GetFrustum() : nullptr;

        {
            LEV_PROFILE_SCOPE("Batch static meshes");

            const auto staticMeshGroup = registry.group<>(entt::get<Transform, MeshRendererComponent>, entt::exclude<AnimatorComponent>);
            for (const auto entity : staticMeshGroup)
            {
                auto [transform, meshRenderer] = staticMeshGroup.get<Transform, MeshRendererComponent>(entity);

                if (!meshRenderer.enabled) continue;
                if (!meshRenderer.mesh) continue;

                auto& material = meshRenderer.material ? meshRenderer.material->GetMaterial() : *Renderer3D::MissingMaterial;
                if (material.IsTransparent()) continue;

                m_Batcher.Add(meshRenderer.mesh->GetMesh(), &material, transform, frustum);
            }
        }

        m_InstancedShader->Bind();

        Material* previousMaterial{nullptr};

        for (const auto& batch : m_Batcher.GetBatches())
        {
            if (batch.Instances.empty()) continue;

            if (previousMaterial != batch.SurfaceMaterial)
                batch.SurfaceMaterial->Bind(m_InstancedShader);

            Renderer3D::DrawMeshInstanced(batch.Instances, batch.Geometry, m_InstancedShader);

            previousMaterial = batch.SurfaceMaterial;
        }

        if (previousMaterial)
            previousMaterial->Unbind(m_InstancedShader);

        m_InstancedShader->Unbind();
    }

    void OpaquePass::ProcessAnimatedMeshes(entt::registry& registry, const RenderParams& params)
    {
        LEV_PROFILE_FUNCTION();

        const auto& animationShader = ShaderAssets::GBufferPassWithAnimations();
        animationShader->Bind();

        Material* previousMaterial{nullptr};

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
                material.Bind(animationShader);
			}

            Renderer3D::DrawMesh(transform.GetModel(), animator.GetFinalBoneMatrices(), mesh, animationShader);

            previousMaterial = &material;
        }

        if (previousMaterial)
            previousMaterial->Unbind(animationShader);

        animationShader->Unbind();
    }

    void OpaquePass::End(entt::registry& registry, RenderParams& params)
    {
        m_PipelineState->Unbind();
    }
}

