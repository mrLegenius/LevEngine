#include "levpch.h"
#include "EndQueryPass.h"

#include "Query.h"
#include "Kernel/Time/Time.h"

namespace LevEngine
{
    EndQueryPass::EndQueryPass(const Ref<Query>& query) : m_Query(query) { }

    void EndQueryPass::Process(entt::registry& registry, RenderParams& params)
    {
        if (m_Query)
            m_Query->End(Time::GetFrameNumber());
    }
}
