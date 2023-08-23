#pragma once
#include "Kernel/Layer.h"

namespace LevEngine
{
	class ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer() override = default;

		void OnAttach() override;
		void OnDetach() override;
		void OnEvent(Event& event) override;
		void OnGUIRender() override;

		static void SetDarkThemeColors();
		
		void Begin();
		void End();
		void BlockEvents(bool value);
	private:
		bool m_IsBlockingEvents = true;
	};
}
