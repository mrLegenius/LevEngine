#include "levpch.h"
#include "TimelineComponent.h"

#include "Kernel/Time/Timeline.h"
#include "Kernel/Time/TimelineParameters.h"
#include "Scene/Components/ComponentSerializer.h"

namespace LevEngine
{
    TimelineComponent::TimelineComponent()
    {
        duration = 0.0;
        isLooping = false;
        playOnInit = true;
    }

    void TimelineComponent::Init()
    {
        const auto parameters = CreateRef(TimelineParameters{playOnInit, isLooping, duration});
        timeline = new Timeline(parameters);
    }

    double TimelineComponent::GetElapsedTime() const
    {
        if (timeline != nullptr)
        {
            return timeline->GetTimeSinceStartup();
        }

        return 0.0f;
    }

    double TimelineComponent::GetDuration() const
    {
        return duration;
    }

    void TimelineComponent::SetDuration(double duration)
    {
        this->duration = duration;
    }

    class TimelineComponentSerializer final : public ComponentSerializer<TimelineComponent, TimelineComponentSerializer>
    {
    protected:
        const char* GetKey() override { return "TimelineComponent"; } 

        void SerializeData(YAML::Emitter& out, const TimelineComponent& component) override
        {
            out << YAML::Key << "IsLooping" << YAML::Value << component.isLooping;
            out << YAML::Key << "Duration" << YAML::Value << component.duration;
            out << YAML::Key << "PlayOnInit" << YAML::Value << component.playOnInit;
        }
        
        void DeserializeData(YAML::Node& node, TimelineComponent& component) override
        {
            component.isLooping = node["IsLooping"].as<bool>();
            component.duration = node["Duration"].as<double>();
            component.playOnInit = node["PlayOnInit"].as<bool>();
        }
    };
}
