#include "pch.h"

#include "ComponentDrawer.h"
#include "Scene/Components/Audio/AudioListener.h"

namespace LevEngine::Editor
{
	class AudioListenerComponentDrawer final : public ComponentDrawer<AudioListenerComponent, AudioListenerComponentDrawer>
	{
	protected:
		[[nodiscard]] String GetLabel() const override { return "Audio Listener"; }
		String GetAddMenuPath() const override { return "Audio"; }

		void DrawContent(AudioListenerComponent& component) override { }
	};	
}
