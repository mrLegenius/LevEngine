#include <functional>
#include <string>

#include "../Renderer/D3D11Context.h"
#include "../Events/Event.h"
#include <memory>

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

	explicit Window(const WindowAttributes& attributes);
	~Window();

	void Update();

	[[nodiscard]] unsigned int GetWidth() const { return m_Data.width; }
	[[nodiscard]] unsigned int GetHeight() const { return m_Data.height; }

	void SetEventCallback(const EventCallbackFn& callback)
	{
		m_Data.eventCallback = callback;
	}

	void SetWindowTitle(std::string& title);
	void SetVSync(bool enabled);

	void EnableCursor();
	void DisableCursor();

	[[nodiscard]] bool IsVSync() const;

	[[nodiscard]] void* GetNativeWindow() const { return m_Window; }

	static std::unique_ptr<Window> Create(const WindowAttributes& attributes = WindowAttributes());

private:

	void ConfineCursor() const;
	void FreeCursor() const;

	void ShowCursor() const;
	void HideCursor() const;

	virtual void Init(const WindowAttributes& attributes);
	virtual void Close();

	HWND m_Window = nullptr;
	D3D11Context* m_Context = nullptr;
	bool m_CursorEnabled = true;

public:
	struct WindowData
	{
		std::string title;
		unsigned int width = 0;
		unsigned int height = 0;

		bool vSync = false;

		EventCallbackFn eventCallback;
	};

	WindowData m_Data;
};


