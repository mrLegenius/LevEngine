#pragma once
#include "RenderPass.h"

namespace LevEngine
{
    class Query;

    class BeginQueryPass final : public RenderPass
    {
    public:
        explicit BeginQueryPass(const Ref<Query>& query);
    protected:
        void Process(entt::registry& registry, RenderParams& params) override;

    private:
        Ref<Query> m_Query;
    };
}

