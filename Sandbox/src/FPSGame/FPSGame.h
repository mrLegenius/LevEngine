#pragma once

namespace Sandbox
{
	class FPSGame final : public LevEngine::Layer
	{
	public:
		void OnAttach() override;
		void OnDetach() override;
		void OnUpdate(float deltaTime) override;
		void OnRender() override;
		void OnGUIRender() override;
		void OnEvent(LevEngine::Event& event) override;

	private:
		static bool OnWindowFocus(LevEngine::WindowFocusEvent& event);
	};
}
