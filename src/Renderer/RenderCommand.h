#pragma once
#include <cstdint>

#include "D3D11RendererAPI.h"

class RenderCommand
{
public:
	static void Init()
	{
		s_RendererAPI->Init();
	}

	static void SetViewport(int x, int y, uint32_t width, uint32_t height)
	{
		s_RendererAPI->SetViewport(x, y, width, height);
	}

	static void SetClearColor(const float color[4])
	{
		s_RendererAPI->SetClearColor(color);
	}
	static void Clear()
	{
		s_RendererAPI->Clear();
	}

	static void SetLineWidth(float width)
	{
		s_RendererAPI->SetLineWidth(width);
	}

private:
	static D3D11RendererAPI* s_RendererAPI;
};

