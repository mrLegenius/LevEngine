#include "pch.h"

#include "ComponentDrawer.h"
#include "Scene/Components/Time/TimelineComponent.h"

namespace LevEngine::Editor
{
    class TimelineComponentDrawer final : public ComponentDrawer<TimelineComponent, TimelineComponentDrawer>
    {
    protected:
        [[nodiscard]] String GetLabel() const override { return "TimelineComponent"; }

        void DrawContent(TimelineComponent& component) override
        {
            ImGui::Checkbox("Is Looping", &component.isLooping);

            GUIUtils::DrawDoubleControl("Duration",
                                       BindGetter(&TimelineComponent::GetDuration, &component),
                                       BindSetter(&TimelineComponent::SetDuration, &component),
                                       0.1f, 0, std::numeric_limits<double>::max());
            
            ImGui::Checkbox("Play On Init", &component.playOnInit);
        }
    };
}

