#pragma once


namespace Sandbox
{
	class Game : public Layer
	{
	public:
		void OnAttach() override;
		void OnDetach() override;
		void OnUpdate(const float deltaTime) override;
		void OnGUIRender() override;
		void OnEvent(Event& event) override;
	};
}
