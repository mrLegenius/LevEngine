#include "levpch.h"
#include "BeginQueryPass.h"

#include "Query.h"
#include "Kernel/Time/Time.h"

namespace LevEngine
{
    BeginQueryPass::BeginQueryPass(const Ref<Query>& query) : m_Query(query) { }

    void BeginQueryPass::Process(entt::registry& registry, RenderParams& params)
    {
        if (m_Query)
            m_Query->Begin(Time::GetFrameNumber());
    }
}
