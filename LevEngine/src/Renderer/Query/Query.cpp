#include "levpch.h"
#include "Query.h"

#include "Kernel/Application.h"
#include "Renderer/RenderDevice.h"

namespace LevEngine
{
    Ref<Query> Query::Create(QueryType queryType, uint8_t numBuffers)
    {
        return App::RenderDevice().CreateQuery(queryType, numBuffers);
    }
}
