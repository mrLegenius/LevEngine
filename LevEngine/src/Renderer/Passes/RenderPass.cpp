#include "levpch.h"
#include "RenderPass.h"
#include "Renderer/RenderDebugEvent.h"
#include "Renderer/Material/Material.h"
#include "Renderer/Shader/Shader.h"

namespace LevEngine
{
    Ref<Shader> RenderPass::SelectShader(const Material& material, const MaterialShaderVariant& variant,
                                         const Ref<Shader>& passShader)
    {
        const auto materialShader = material.GetShader(variant);

        return materialShader ? materialShader : passShader;
    }

    void RenderPass::BindShader(const Ref<Shader>& shader, Ref<Shader>& boundShader)
    {
        if (shader == boundShader) return;

        if (boundShader)
            boundShader->Unbind();

        if (shader)
            shader->Bind();

        boundShader = shader;
    }


    bool RenderPass::Execute(entt::registry& registry, RenderParams& params)
    {
        LEV_PROFILE_FUNCTION();
        
        if (Begin(registry, params))
        {
            const auto passName = PassName();
            
            if (!passName.empty())
                RenderDebugEvent::Start(passName);
            
            Process(registry, params);
            
            if (!passName.empty())
                RenderDebugEvent::End();
			
            End(registry, params);

            return true;
        }

        return false;
    }
}
