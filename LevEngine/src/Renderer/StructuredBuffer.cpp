#include "pch.h"
#include "StructuredBuffer.h"

#include "RenderSettings.h"
#include "Platform/D3D11/D3D11StructuredBuffer.h"

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
		switch (RenderSettings::RendererAPI)
		{
		case RendererAPI::None:
			LEV_THROW("None for API was chosen")
		case RendererAPI::OpenGL:
			LEV_NOT_IMPLEMENTED
		case RendererAPI::D3D11:
			return CreateRef<D3D11StructuredBuffer>(data, count, stride, cpuAccess, uav, uavType);
		default:
			LEV_THROW("Unknown Renderer API")
			break;
		}
	}
}
