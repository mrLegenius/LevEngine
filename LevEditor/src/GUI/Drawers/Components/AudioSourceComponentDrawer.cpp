#include "pch.h"

#include "ComponentDrawer.h"
#include "Scene/Components/Audio/AudioSource.h"

namespace LevEngine::Editor
{
    class AudioSourceComponentDrawer final : public ComponentDrawer<AudioSourceComponent, AudioSourceComponentDrawer>
    {
    protected:
        [[nodiscard]] String GetLabel() const override { return "Audio Source"; }

        void DrawContent(AudioSourceComponent& component) override
        {
            GUIUtils::DrawAsset<AudioBankAsset>("Audio Bank", component.audioBank);

            auto& eventName = component.eventName;
            ImGui::InputText("Event name", eventName.data(), 256);

            GUIUtils::DrawCheckBox("Play On Init", BindGetter(&AudioSourceComponent::GetPlayOnInit, &component),
                BindSetter(&AudioSourceComponent::SetPlayOnInit, &component));
        }
    };	
}
