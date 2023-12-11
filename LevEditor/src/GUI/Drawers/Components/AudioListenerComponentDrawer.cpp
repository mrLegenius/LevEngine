#include "pch.h"

#include "ComponentDrawer.h"
#include "Scene/Components/Audio/AudioListener.h"

namespace LevEngine::Editor
{
	class AudioListenerComponentDrawer final : public ComponentDrawer<AudioListenerComponent, AudioListenerComponentDrawer>
	{
	protected:
		[[nodiscard]] String GetLabel() const override { return "Audio/Audio Listener"; }

		void DrawContent(AudioListenerComponent& component) override { }
	};	
}
