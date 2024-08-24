#include "levpch.h"
#include "ConstantBuffer.h"

#include "Kernel/Application.h"
#include "Renderer/RenderDevice.h"

namespace LevEngine
{
Ref<ConstantBuffer> ConstantBuffer::Create(uint32_t size, uint32_t slot)
{
	return App::RenderDevice().CreateConstantBuffer(size, slot);
}
}

