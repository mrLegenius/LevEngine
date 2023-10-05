#include "pch.h"

#include "ComponentDrawer.h"
#include "Scene/Components/Time/TimelineComponent.h"

namespace LevEngine::Editor
{
    class TimelineComponentDrawer final : public ComponentDrawer<TimelineComponent, TimelineComponentDrawer>
    {
    protected:
        [[nodiscard]] String GetLabel() const override { return "Timeline"; }

        void DrawContent(TimelineComponent& component) override
        {
            EditorGUI::DrawCheckBox("Is Looping", BindGetter(&TimelineComponent::GetIsLooping, &component),
                BindSetter(&TimelineComponent::SetIsLooping, &component));

            constexpr double max = std::numeric_limits<double>::max();
            EditorGUI::DrawDoubleControl("Duration",
                                        BindGetter(&TimelineComponent::GetDuration, &component),
                                        BindSetter(&TimelineComponent::SetDuration, &component),
                                        0.1f, 0, max);

            EditorGUI::DrawDoubleControl("Time Scale",
                                       BindGetter(&TimelineComponent::GetTimeScale, &component),
                                       BindSetter(&TimelineComponent::SetTimeScale, &component),
                                       0.01f);
            
            EditorGUI::DrawCheckBox("Play On Init", BindGetter(&TimelineComponent::GetPlayOnInit, &component),
                BindSetter(&TimelineComponent::SetPlayOnInit, &component));

            // TODO: Draw control buttons only if editor is in Play mode.
            if (!component.IsPlaying())
            {
                if (ImGui::Button("Play", {80, 50}))
                {
                    component.Play();
                }
            }
            else
            {
                if (ImGui::Button("Pause", {80, 50}))
                {
                    component.Pause();
                }
                ImGui::SameLine();

                if (ImGui::Button("Stop", {80, 50}))
                {
                    component.Stop();
                }
            }
        }
    };
}

