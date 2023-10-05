#pragma once


namespace Sandbox
{
	class Game : public Layer
	{
	public:
		void OnAttach() override;
		void OnDetach() override;
		void OnUpdate(float deltaTime) override;
		void OnRender() override;
		void OnGUIRender() override;
		void OnEvent(Event& event) override;
	};
}
