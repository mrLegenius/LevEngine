#pragma once

#include "Kernel/Core.h"

namespace LevEngine
{
	class IndexBuffer;

	class LEV_API RenderCommands
	{
	public:
		virtual ~RenderCommands() = default;

		static Ref<RenderCommands> Create();

		virtual void DrawIndexed(const Ref<IndexBuffer>& indexBuffer) = 0;
		virtual void DrawIndexedInstanced(const Ref<IndexBuffer>& indexBuffer, uint32_t instanceCount) = 0;
		virtual void DrawFullScreenQuad() = 0;
		virtual void DrawPointList(uint32_t count) = 0;
		virtual void DrawLineStrip(uint32_t vertexCount) = 0;
		virtual void DrawLineList(const Ref<IndexBuffer>& indexBuffer) = 0;
	};
}
