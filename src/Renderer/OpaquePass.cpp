#include "pch.h"
#include "OpaquePass.h"

#include "RenderCommand.h"
#include "Renderer3D.h"
#include "Kernel/Application.h"
#include "Scene/Components/Components.h"

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
        m_CameraConstantBuffer->Bind(Shader::Type::Vertex | Shader::Type::Pixel);
        return RenderPass::Begin(registry, params);
    }

    void OpaquePass::Process(entt::registry& registry, RenderParams& params)
    {
	    const auto shader = m_PipelineState->GetShader(Shader::Type::Vertex);
        const auto view = registry.group<>(entt::get<Transform, MeshRendererComponent>);
        for (const auto entity : view)
        {
            auto [transform, meshRenderer] = view.get<Transform, MeshRendererComponent>(entity);

            if (!meshRenderer.mesh->VertexBuffer)
                meshRenderer.mesh->Init(shader->GetLayout());

            meshRenderer.material.Bind(shader);
            Renderer3D::DrawMesh(transform.GetModel(), meshRenderer);
            meshRenderer.material.Unbind(shader);
        }
    }

    void OpaquePass::End(entt::registry& registry, RenderParams& params)
    {
        m_PipelineState->Unbind();
    }
}

