#include "levpch.h"
#include "EndQueryPass.h"

#include "Kernel/Time/Time.h"
#include "Renderer/Query.h"

namespace LevEngine
{
    EndQueryPass::EndQueryPass(const Ref<Query>& query) : m_Query(query) { }

    void EndQueryPass::Process(entt::registry& registry, RenderParams& params)
    {
        if (m_Query)
            m_Query->End(Time::GetFrameNumber());
    }
}
