#pragma once
#include "IndexBuffer.h"
#include "VertexBuffer.h"

namespace LevEngine
{
	class RenderCommands
	{
	public:
		virtual ~RenderCommands() = default;

		static Ref<RenderCommands> Create();

		virtual void DrawIndexed(const Ref<IndexBuffer>& indexBuffer) = 0;
		virtual void DrawFullScreenQuad() = 0;
		virtual void DrawPointList(uint32_t count) = 0;
	};
}
