#include "levpch.h"
#include "RenderPass.h"
#include "Renderer/RenderDebugEvent.h"
#include "Renderer/RenderStatistics.h"
#include "Renderer/Material/Material.h"
#include "Renderer/Shader/Shader.h"

namespace LevEngine
{
    namespace
    {
        // Half the passes leave PassName() empty because only the GPU debugger reads it, and a
        // statistics table with a dozen blank rows is useless. The C++ type name is always there.
        String StatisticsName(const RenderPass& pass, const String& passName)
        {
            if (!passName.empty()) return passName;

            String name = typeid(pass).name();

            const auto lastSpace = name.find_last_of(' ');
            if (lastSpace != String::npos)
                name = name.substr(lastSpace + 1);

            const auto lastColon = name.find_last_of(':');
            if (lastColon != String::npos)
                name = name.substr(lastColon + 1);

            return name;
        }
    }

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

            //<--- Naming a pass costs a string, so it is only paid for while something is counting ---<<
            if (RenderStatistics::IsEnabled())
            {
                const ScopedRenderPassStatistics statistics{ StatisticsName(*this, passName) };
                Process(registry, params);
            }
            else
            {
                Process(registry, params);
            }

            if (!passName.empty())
                RenderDebugEvent::End();
			
            End(registry, params);

            return true;
        }

        return false;
    }
}
