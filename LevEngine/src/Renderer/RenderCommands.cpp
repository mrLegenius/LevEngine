#include "levpch.h"
#include "RenderCommands.h"

#include "RenderDevice.h"
#include "Kernel/Application.h"

namespace LevEngine
{
	Ref<RenderCommands> RenderCommands::Create()
	{
		return App::RenderDevice().CreateRenderCommands();
	}
}
