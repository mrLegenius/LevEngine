#pragma once

#include "Kernel/Core.h"
#include <sstream>
#include "Event.h"
#include "../Input/KeyCodes.h"
namespace LevEngine
{
class LEV_API KeyEvent : public Event
{
public:
	[[nodiscard]] KeyCode GetKeyCode() const { return m_Keycode; }

	EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)
protected:
	explicit KeyEvent(const KeyCode keycode) : m_Keycode(keycode) { }

	KeyCode m_Keycode;
};

class LEV_API KeyPressedEvent : public KeyEvent
{
public:
	KeyPressedEvent(const KeyCode keycode, const int repeatCount)
		: KeyEvent(keycode), m_RepeatCount(repeatCount) { }

	[[nodiscard]] int GetRepeatCount() const { return m_RepeatCount; }

	[[nodiscard]] String ToString() const override
	{
		std::stringstream ss;
		ss << "KeyPressedEvent: " << static_cast<int>(m_Keycode) << " (" << m_RepeatCount << " repeats)";
		return ss.str().c_str();
	}

	EVENT_CLASS_TYPE(KeyPressed)

private:
	int m_RepeatCount;
};

class LEV_API KeyTypedEvent : public KeyEvent
{
public:
	KeyTypedEvent(const KeyCode keycode)
		: KeyEvent(keycode) { }

	[[nodiscard]] String ToString() const override
	{
		std::stringstream ss;
		ss << "KeyTypedEvent: " << static_cast<int>(m_Keycode);
		return ss.str().c_str();
	}

	EVENT_CLASS_TYPE(KeyTyped)
};

class LEV_API KeyReleasedEvent : public KeyEvent
{
public:
	explicit KeyReleasedEvent(const KeyCode keycode)
		: KeyEvent(keycode) { }

	[[nodiscard]] String ToString() const override
	{
		std::stringstream ss;
		ss << "KeyReleasedEvent: " << static_cast<int>(m_Keycode);
		return ss.str().c_str();
	}

	EVENT_CLASS_TYPE(KeyReleased)
};
}