#pragma once
#include "Renderer/Passes/RenderPass.h"

namespace LevEngine
{
    class Query;

    class EndQueryPass final : public RenderPass
    {
    public:
        explicit EndQueryPass(const Ref<Query>& query);
    protected:
        void Process(entt::registry& registry, RenderParams& params) override;

    private:
        Ref<Query> m_Query;
    };
}
