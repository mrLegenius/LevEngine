#pragma once
#include <string>

#include "../Events/Event.h"
namespace LevEngine
{
class Layer
{
public:
	explicit Layer(std::string name = "Layer");
	virtual ~Layer() = default;

	virtual void OnAttach() { }
	virtual void OnUpdate(const float deltaTime) { }
	virtual void OnEvent(Event& event) { }

	[[nodiscard]] const std::string& GetName() const { return m_Name; }
protected:
	std::string m_Name;
};
}