#pragma once
#include <sstream>
#include "Event.h"
#include "../Input/KeyCodes.h"

class KeyEvent : public Event
{
public:
	[[nodiscard]] KeyCode GetKeyCode() const { return m_Keycode; }

	EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)
protected:
	explicit KeyEvent(const KeyCode keycode) : m_Keycode(keycode) { }

	KeyCode m_Keycode;
};

class KeyPressedEvent : public KeyEvent
{
public:
	KeyPressedEvent(const KeyCode keycode, const int repeatCount)
		: KeyEvent(keycode), m_RepeatCount(repeatCount) { }

	[[nodiscard]] int GetRepeatCount() const { return m_RepeatCount; }

	[[nodiscard]] std::string ToString() const override
	{
		std::stringstream ss;
		ss << "KeyPressedEvent: " << static_cast<int>(m_Keycode) << " (" << m_RepeatCount << " repeats)";
		return ss.str();
	}

	EVENT_CLASS_TYPE(KeyPressed)

private:
	int m_RepeatCount;
};

class KeyTypedEvent : public KeyEvent
{
public:
	KeyTypedEvent(const KeyCode keycode)
		: KeyEvent(keycode) { }

	[[nodiscard]] std::string ToString() const override
	{
		std::stringstream ss;
		ss << "KeyTypedEvent: " << static_cast<int>(m_Keycode);
		return ss.str();
	}

	EVENT_CLASS_TYPE(KeyTyped)
};

class KeyReleasedEvent : public KeyEvent
{
public:
	explicit KeyReleasedEvent(const KeyCode keycode)
		: KeyEvent(keycode) { }

	[[nodiscard]] std::string ToString() const override
	{
		std::stringstream ss;
		ss << "KeyReleasedEvent: " << static_cast<int>(m_Keycode);
		return ss.str();
	}

	EVENT_CLASS_TYPE(KeyReleased)
};