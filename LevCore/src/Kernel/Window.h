#pragma once
#include <string>
#include "PointerUtils.h"
#include "../Events/Event.h"

namespace LevEngine
{
	struct WindowAttributes
	{
		std::string title;
		uint32_t width;
		uint32_t height;

		explicit WindowAttributes(std::string title = "LevEngine",
			const unsigned int width = 1280,
			const unsigned int height = 720)
			: title(std::move(title)),
			width(width),
			height(height) { }
	};
	
	class Window
	{
	public:
		using EventCallbackFn = std::function<void(Event&)>;

		virtual ~Window() { }

		virtual void Update() = 0;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;

		virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
		virtual void SetVSync(bool enabled) = 0;
		virtual bool IsVSync() const = 0;

		virtual void* GetNativeWindow() const = 0;

		static Scope<Window> Create(const WindowAttributes& attributes = WindowAttributes());
	};
}