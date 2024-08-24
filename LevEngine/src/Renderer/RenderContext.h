#pragma once
#include "RenderDevice.h"
#include "3D/MeshLoading/AnimationLoader.h"

namespace LevEngine
{
	class RenderTarget;

	class RenderContext
	{
	public:
		RenderContext() = default;
		virtual ~RenderContext() = default;

		static Ref<RenderContext> Create();

		virtual void Init(Ref<RenderDevice> renderDevice, const uint32_t width, const uint32_t height, bool isVSync, const HWND window) = 0;
		virtual void SwapBuffers(bool isVSync) = 0;
		virtual void ResizeBackBuffer(uint16_t width, uint16_t height) = 0;

		const Ref<RenderTarget>& GetRenderTarget() const { return m_RenderTarget; }

	protected:
		Ref<RenderTarget> m_RenderTarget{};
	};
}
