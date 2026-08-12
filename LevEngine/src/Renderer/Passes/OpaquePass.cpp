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
    OpaquePass::OpaquePass(const Ref<PipelineState>& pipelineState, const Ref<Shader>& instancedShader,
                           const bool deferred)
        : m_PipelineState(pipelineState), m_InstancedShader(instancedShader), m_Deferred(deferred) { }

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
                                         const Ref<Shader>& passShader) const
    {
        LEV_PROFILE_FUNCTION();

        const MaterialShaderVariant variant{m_Deferred, false, false};

        Material* previousMaterial{nullptr};

        //<--- Begin() bound the pipeline, which bound the pass shader ---<<
        Ref<Shader> boundShader = passShader;

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

            const auto shader = SelectShader(material, variant, passShader);

            //<--- The material is bound into the shader, so a shader swap rebinds it too ---<<
            if (previousMaterial != &material || shader != boundShader)
            {
                if (previousMaterial && shader != boundShader)
                    previousMaterial->Unbind(boundShader);

                BindShader(shader, boundShader);
                material.Bind(shader);
            }

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
            previousMaterial->Unbind(boundShader);

        //<--- End() unbinds the pipeline, so leave its shader as the bound one ---<<
        BindShader(passShader, boundShader);
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

        const MaterialShaderVariant variant{m_Deferred, true, false};

        Ref<Shader> boundShader;
        BindShader(m_InstancedShader, boundShader);

        Material* previousMaterial{nullptr};

        for (const auto& batch : m_Batcher.GetBatches())
        {
            if (batch.Instances.empty()) continue;

            const auto shader = SelectShader(*batch.SurfaceMaterial, variant, m_InstancedShader);

            if (previousMaterial != batch.SurfaceMaterial || shader != boundShader)
            {
                if (previousMaterial && shader != boundShader)
                    previousMaterial->Unbind(boundShader);

                BindShader(shader, boundShader);
                batch.SurfaceMaterial->Bind(shader);
            }

            Renderer3D::DrawMeshInstanced(batch.Instances, batch.Geometry, shader);

            previousMaterial = batch.SurfaceMaterial;
        }

        if (previousMaterial)
            previousMaterial->Unbind(boundShader);

        BindShader(nullptr, boundShader);
    }

    void OpaquePass::ProcessAnimatedMeshes(entt::registry& registry, const RenderParams& params) const
    {
        LEV_PROFILE_FUNCTION();

        const auto& animationShader = ShaderAssets::GBufferPassWithAnimations();
        const MaterialShaderVariant variant{m_Deferred, false, true};

        Ref<Shader> boundShader;
        BindShader(animationShader, boundShader);

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

            const auto shader = SelectShader(material, variant, animationShader);

            if (previousMaterial != &material || shader != boundShader)
			{
                if (previousMaterial && shader != boundShader)
                    previousMaterial->Unbind(boundShader);

                BindShader(shader, boundShader);
                material.Bind(shader);
			}

            Renderer3D::DrawMesh(transform.GetModel(), animator.GetFinalBoneMatrices(), mesh, shader);

            previousMaterial = &material;
        }

        if (previousMaterial)
            previousMaterial->Unbind(boundShader);

        BindShader(nullptr, boundShader);
    }

    void OpaquePass::End(entt::registry& registry, RenderParams& params)
    {
        m_PipelineState->Unbind();
    }
}

