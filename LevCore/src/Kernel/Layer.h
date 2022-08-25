#pragma once
#include <string>
#include "../Events/Event.h"

namespace LevEngine
{
	class Layer
	{
	public:
		explicit Layer(const std::string& name = "Layer");
		virtual ~Layer();

		virtual void OnAttach() { }
		virtual void OnDetach() { }
		virtual void OnUpdate() { }
		virtual void OnImGuiRender() { }
		virtual void OnEvent(Event& event) { }

		const std::string& GetName() const { return m_Name; }
	protected:
		std::string m_Name;
	};
}