#include "levpch.h"
#include "RenderDebugEvent.h"

#include "RenderDevice.h"
#include "Kernel/Application.h"

namespace LevEngine
{
    void RenderDebugEvent::Start(const String& label) { s_Instance->StartEvent(label); }
    void RenderDebugEvent::End() { s_Instance->EndEvent(); }

    Ref<RenderDebugEvent> RenderDebugEvent::Create()
    {
        return App::RenderDevice().CreateRenderDebugEvent();
    }
}
