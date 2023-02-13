#include <string>

#include "../Renderer/D3D11Context.h"
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
	explicit Window(const WindowAttributes& attributes);
	~Window();

	void Update();

	[[nodiscard]] unsigned int GetWidth() const { return m_Data.width; }
	[[nodiscard]] unsigned int GetHeight() const { return m_Data.height; }

	void SetWindowTitle(std::string& title);
	void SetVSync(bool enabled);
	[[nodiscard]] bool IsVSync() const;

	[[nodiscard]] void* GetNativeWindow() const { return m_Window; }

	static std::unique_ptr<Window> Create(const WindowAttributes& attributes = WindowAttributes());

private:
	virtual void Init(const WindowAttributes& attributes);
	virtual void Close();

	HWND m_Window = nullptr;
	D3D11Context* m_Context = nullptr;

	struct WindowData
	{
		std::string title;
		unsigned int width = 0;
		unsigned int height = 0;

		bool vSync = false;
	};

	WindowData m_Data;
};


