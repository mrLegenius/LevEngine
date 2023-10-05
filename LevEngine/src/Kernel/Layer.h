#pragma once

#include "../Events/Event.h"

namespace LevEngine
{
class Layer
{
public:
	explicit Layer(String name = "Layer");
	virtual ~Layer() = default;

	virtual void OnAttach() { }
	virtual void OnDetach() { }
	virtual void OnUpdate(const float deltaTime) { }
	virtual void OnRender() { }
	virtual void OnGUIRender() { }
	virtual void OnEvent(Event& event) { }

	[[nodiscard]] const String& GetName() const { return m_Name; }
protected:
	String m_Name;
};
}