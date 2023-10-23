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
            Ref<AudioPlayer> audioPlayer = component.audioPlayer;
            EditorGUI::DrawAsset<AudioBankAsset>("Audio Bank", audioPlayer->GetAudioBankAsset());

            auto& eventName = audioPlayer->GetEventName();
            char buffer[256] = {};
            strcpy_s(buffer, sizeof buffer, eventName.c_str());
            if (ImGui::InputText("Event name", buffer, sizeof(buffer), ImGuiInputTextFlags_EnterReturnsTrue))
            {
                audioPlayer->SetEventName(String(buffer));
            }

            EditorGUI::DrawCheckBox("Play On Init", BindGetter(&AudioPlayer::GetPlayOnInit, audioPlayer),
                BindSetter(&AudioPlayer::SetPlayOnInit, audioPlayer));

            EditorGUI::DrawCheckBox("Is One-Shot", BindGetter(&AudioPlayer::GetIsOneShot, audioPlayer),
                BindSetter(&AudioPlayer::SetIsOneShot, audioPlayer));
        }
    };	
}
