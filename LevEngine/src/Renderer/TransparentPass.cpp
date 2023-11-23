#include "levpch.h"
#include "TransparentPass.h"

#include "Renderer3D.h"
#include "RenderSettings.h"
#include "Scene/Components/MeshRenderer/MeshRenderer.h"

namespace LevEngine
{
    TransparentPass::TransparentPass(const Ref<PipelineState>& pipelineState) : m_PipelineState(pipelineState) {  }

    bool TransparentPass::Begin(entt::registry& registry, RenderParams& params)
    {
        m_PipelineState->Bind();
        
        return RenderPass::Begin(registry, params);
    }

    void TransparentPass::Process(entt::registry& registry, RenderParams& params)
    {
        const auto view = registry.group<>(entt::get<Transform, MeshRendererComponent>);
        const auto shader = m_PipelineState->GetShader(ShaderType::Vertex);
        for (const auto entity : view)
        {
            Transform transform = view.get<Transform>(entity);
            const MeshRendererComponent meshRenderer = view.get<MeshRendererComponent>(entity);

            if (!meshRenderer.material) continue;
            auto& material = meshRenderer.material->GetMaterial();
            
            if (!material.IsTransparent()) continue;
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
    }

    void TransparentPass::End(entt::registry& registry, RenderParams& params)
    {
        m_PipelineState->Unbind();
    }
}
