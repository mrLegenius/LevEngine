#pragma once
#include "Panel.h"
#include "Undo/EntityEditTracker.h"

namespace LevEngine::Editor
{
	class PropertiesPanel final : public Panel
	{
	public:
		PropertiesPanel() { m_DefaultWindowSize = Vector2{ 555, 660 }; }

	protected:
		String GetName() override { return "Properties"; }
		void DrawContent() override;

	private:
		//<--- Inspector fields write straight into the components, so what they changed is found by
		//comparing the entity with what it looked like before the widget was grabbed ---<<
		EntityEditTracker m_EditTracker{ "Edit" };
	};
}

