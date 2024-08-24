#include "levpch.h"
#include "DispatchCommands.h"

#include "Kernel/Application.h"
#include "Renderer/RenderDevice.h"

namespace LevEngine
{
    Ref<DispatchCommands> DispatchCommands::Create()
    {
        return App::RenderDevice().CreateDispatchCommands();
    }
}
