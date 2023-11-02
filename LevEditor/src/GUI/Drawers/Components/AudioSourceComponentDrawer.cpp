#include "pch.h"

#include "ComponentDrawer.h"

#include "Audio/AudioPlayer.h"
#include "Scene/Components/Audio/AudioSource.h"

namespace LevEngine::Editor
{
    class AudioSourceComponentDrawer final : public ComponentDrawer<AudioSourceComponent, AudioSourceComponentDrawer>
    {
    protected:
        [[nodiscard]] String GetLabel() const override { return "Audio Source"; }

        void DrawContent(AudioSourceComponent& component) override
        {
            EditorGUI::DrawAsset<AudioBankAsset>("Audio Bank", component.Player->GetAudioBankAsset());

            auto& eventName = component.Player->GetEventName();
            char buffer[256] = {};
            strcpy_s(buffer, sizeof buffer, eventName.c_str());
            if (ImGui::InputText("Event name", buffer, sizeof(buffer), ImGuiInputTextFlags_EnterReturnsTrue))
            {
                component.Player->SetEventName(String(buffer));
            }

            EditorGUI::DrawCheckBox("Play On Init", BindGetter(&AudioPlayer::GetPlayOnInit, component.Player),
                BindSetter(&AudioPlayer::SetPlayOnInit, component.Player));

            EditorGUI::DrawCheckBox("Is One-Shot", BindGetter(&AudioPlayer::GetIsOneShot, component.Player),
                BindSetter(&AudioPlayer::SetIsOneShot, component.Player));
        }
    };	
}
