#pragma once

#include "Kernel/Core.h"
#include "Renderer/Passes/RenderPass.h"

namespace LevEngine
{
    class Query;

    class LEV_API EndQueryPass final : public RenderPass
    {
    public:
        explicit EndQueryPass(const Ref<Query>& query);
    protected:
        void Process(entt::registry& registry, RenderParams& params) override;

    private:
        Ref<Query> m_Query;
    };
}
