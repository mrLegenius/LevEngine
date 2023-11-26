#pragma once

namespace LevEngine
{
	class RenderTarget;

	class RendererContext
	{
	public:
		RendererContext() = default;
		virtual ~RendererContext() = default;

		static Ref<RendererContext> Create();

		virtual void Init(uint32_t width, uint32_t height, HWND window) = 0;
		virtual void SwapBuffers() = 0;
		virtual void ResizeBackBuffer(uint16_t width, uint16_t height) = 0;

		const Ref<RenderTarget>& GetRenderTarget() const { return m_RenderTarget; }

	protected:
		Ref<RenderTarget> m_RenderTarget{};
	};
}
