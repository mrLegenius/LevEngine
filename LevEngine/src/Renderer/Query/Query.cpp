#include "levpch.h"
#include "Query.h"

#include "Platform/D3D11/D3D11Query.h"
#include "Renderer/RenderSettings.h"

namespace LevEngine
{
    Ref<Query> Query::Create(QueryType queryType, uint8_t numBuffers)
    {
        switch (RenderSettings::RendererAPI)
        {
        case RendererAPI::None:
            LEV_THROW("None for API was chosen")
        case RendererAPI::OpenGL:
            LEV_NOT_IMPLEMENTED
        case RendererAPI::D3D11:
            return CreateRef<D3D11Query>(queryType, numBuffers);
        default:
            LEV_THROW("Unknown Renderer API")
            break;
        }
    }
}
