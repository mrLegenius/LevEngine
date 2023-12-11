#include "levpch.h"
#include "RenderPass.h"
#include "RenderDebugEvent.h"

namespace LevEngine
{
    bool RenderPass::Execute(entt::registry& registry, RenderParams& params)
    {
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
