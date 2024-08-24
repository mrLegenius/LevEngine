#include "levpch.h"
#include "RenderContext.h"

#include "RenderDevice.h"
#include "Kernel/Application.h"

namespace LevEngine
{
	Ref<RenderContext> RenderContext::Create()
	{
		return App::RenderDevice().CreateRenderContext();
	}
}
