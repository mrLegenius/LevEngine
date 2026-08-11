#pragma once

#include "Kernel/Core.h"
#include "Passes/RenderPass.h"

namespace LevEngine
{
    class LEV_API RenderTechnique
    {
    public:
        RenderTechnique() = default;

        void AddPass(Ref<RenderPass> renderPass);
        void Process(entt::registry& registry, RenderParams params) const;

        void SetViewport(const Viewport& viewport);

    private:
        using PassList = Vector<Ref<RenderPass>>;

        PassList m_PassList;
    };
}
