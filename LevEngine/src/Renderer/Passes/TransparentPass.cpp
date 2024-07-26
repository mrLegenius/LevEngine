#include "levpch.h"
#include "TransparentPass.h"

#include "Renderer/Material//Material.h"
#include "Renderer/Pipeline/PipelineState.h"
#include "Renderer/Renderer3D.h"
#include "Renderer/RenderParams.h"
#include "Renderer/RenderSettings.h"
#include "Renderer/Shader/ShaderType.h"
#include "Renderer/3D/Mesh.h"
#include "Assets/MaterialAsset.h"
#include "Assets/MeshAsset.h"
#include "Renderer/Camera/SceneCamera.h"
#include "Scene/Components/MeshRenderer/MeshRenderer.h"
#include "Scene/Components/Transform/Transform.h"

namespace LevEngine
{
    TransparentPass::TransparentPass(const Ref<PipelineState>& pipelineState) : m_PipelineState(pipelineState) {  }

    String TransparentPass::PassName() { return "Transparent"; }

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

            if (!meshRenderer.enabled) continue;
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
