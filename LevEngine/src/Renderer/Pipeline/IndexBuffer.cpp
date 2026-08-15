#include "levpch.h"
#include "IndexBuffer.h"

#include "Kernel/Application.h"
#include "Renderer/RenderDevice.h"
#include "Renderer/RenderStatistics.h"

namespace LevEngine
{
	Ref<IndexBuffer> IndexBuffer::Create(const uint32_t* indices, uint32_t count)
	{
		RenderStatistics::CountIndexBufferCreated();
		return App::RenderDevice().CreateIndexBuffer(indices, count);
	}
}
