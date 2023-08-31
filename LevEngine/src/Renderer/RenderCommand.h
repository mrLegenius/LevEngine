#pragma once
#include "RenderCommands.h"

namespace LevEngine
{
class RenderCommand
{
public:
	static void DrawIndexed(
		const Ref<IndexBuffer>& indexBuffer)
	{
		s_RendererAPI->DrawIndexed(indexBuffer);
	}

	static void DrawFullScreenQuad()
	{
		s_RendererAPI->DrawFullScreenQuad();
	}

	static void DrawPointList(const uint32_t count)
	{
		s_RendererAPI->DrawPointList(count);
	}

private:
	static inline Ref<RenderCommands> s_RendererAPI = RenderCommands::Create();
};
}
