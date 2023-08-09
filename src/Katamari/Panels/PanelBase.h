#pragma once

namespace LevEngine::Editor
{
	class Panel
	{
	public:
		virtual ~Panel() = default;
		void Render();

	protected:
		virtual std::string GetName() = 0;
		virtual void DrawContent() = 0;

		bool m_Focused = false;
		bool m_Hovered = false;
	};
}

