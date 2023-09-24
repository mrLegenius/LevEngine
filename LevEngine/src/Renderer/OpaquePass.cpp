#include "levpch.h"
#include "OpaquePass.h"

#include "Renderer3D.h"
#include "Scene/Components/Transform/Transform.h"

namespace LevEngine
{
    bool OpaquePass::Begin(entt::registry& registry, RenderParams& params)
    {
        //TODO: Move this to its own pass
        const auto view = registry.group<>(entt::get<Transform, DirectionalLightComponent>);
        for (const auto entity : view)
        {
            auto [transform, light] = view.get<Transform, DirectionalLightComponent>(entity);

            Renderer3D::SetDirLight(transform.GetForwardDirection(), light);
        }

        m_PipelineState->Bind();

        const auto viewProjection = params.CameraViewMatrix * params.Camera.GetProjection();

        const CameraData cameraData{ params.CameraViewMatrix, viewProjection, params.CameraPosition };
        m_CameraConstantBuffer->SetData(&cameraData);
        m_CameraConstantBuffer->Bind(ShaderType::Vertex | ShaderType::Pixel);
        return RenderPass::Begin(registry, params);
    }

    void OpaquePass::Process(entt::registry& registry, RenderParams& params)
    {
        const auto view = registry.group<>(entt::get<Transform, MeshRendererComponent>);
        const auto shader = m_PipelineState->GetShader(ShaderType::Vertex);
        for (const auto entity : view)
        {
            Transform transform = view.get<Transform>(entity);
            const MeshRendererComponent meshRenderer = view.get<MeshRendererComponent>(entity);

            if (!meshRenderer.mesh) continue;

            const auto mesh = meshRenderer.mesh->GetMesh();
            if (!mesh) continue;
            if (!meshRenderer.material) continue;

            if constexpr (RenderSettings::UseFrustumCulling)
            {
                if (!mesh->IsOnFrustum(params.Camera.GetFrustum(), transform)) continue;
            }
            
            auto& material = meshRenderer.material->GetMaterial();
            material.Bind(shader);
            Renderer3D::DrawMesh(transform.GetModel(), mesh, shader);
            material.Unbind(shader);
        }
    }

    void OpaquePass::End(entt::registry& registry, RenderParams& params)
    {
        m_PipelineState->Unbind();
    }
}

