#include "levpch.h"
#include "RenderDebugEvent.h"

#include "RenderSettings.h"
#include "Platform/D3D11/D3D11RenderDebugEvent.h"

namespace LevEngine
{
    void RenderDebugEvent::Start(const String& label) { s_RendererAPI->StartEvent(label); }
    void RenderDebugEvent::End() { s_RendererAPI->EndEvent(); }

    Ref<RenderDebugEvent> RenderDebugEvent::Create()
    {
        switch (RenderSettings::RendererAPI)
        {
        case RendererAPI::None:
            LEV_THROW("None for API was chosen");
        case RendererAPI::OpenGL:
            LEV_NOT_IMPLEMENTED
        case RendererAPI::D3D11:
            return CreateRef<D3D11RenderDebugEvent>();
        default:
            LEV_THROW("Unknown Renderer API")
            break;
        }
    }
}
