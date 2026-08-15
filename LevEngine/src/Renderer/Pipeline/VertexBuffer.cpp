#include "levpch.h"
#include "VertexBuffer.h"

#include "Kernel/Application.h"
#include "Renderer/RenderDevice.h"
#include "Renderer/RenderStatistics.h"

namespace LevEngine
{
	Ref<VertexBuffer> VertexBuffer::Create(const uint32_t size, const uint32_t stride)
	{
		RenderStatistics::CountVertexBufferCreated();
		return App::RenderDevice().CreateVertexBuffer(size, stride);
	}

	Ref<VertexBuffer> VertexBuffer::Create(const float* vertices, const uint32_t size, const uint32_t stride)
	{
		RenderStatistics::CountVertexBufferCreated();
		return App::RenderDevice().CreateVertexBuffer(vertices, size, stride);
	}

	Ref<VertexBuffer> VertexBuffer::Create(const int* vertices, const uint32_t size, const uint32_t stride)
	{
		RenderStatistics::CountVertexBufferCreated();
		return App::RenderDevice().CreateVertexBuffer(vertices, size, stride);
	}
}
