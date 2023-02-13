#pragma once
#include <string>

class Layer
{
public:
	explicit Layer(std::string name = "Layer");
	virtual ~Layer() = default;

	virtual void OnAttach() { }
	virtual void OnUpdate() { }

	[[nodiscard]] const std::string& GetName() const { return m_Name; }
protected:
	std::string m_Name;
};