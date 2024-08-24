#include "levpch.h"
#include "StructuredBuffer.h"

#include "Kernel/Application.h"
#include "Renderer/RenderDevice.h"

namespace LevEngine
{
	Ref<StructuredBuffer> StructuredBuffer::Create(
		const void* data,
		size_t count,
		uint32_t stride, 
		CPUAccess cpuAccess, 
		bool uav, 
		UAVType uavType)
	{
		return App::RenderDevice().CreateStructuredBuffer(data, count, stride, cpuAccess, uav, uavType);
	}
}
