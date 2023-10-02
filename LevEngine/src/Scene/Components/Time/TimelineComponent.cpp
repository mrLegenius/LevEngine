#include "levpch.h"
#include "TimelineComponent.h"

#include "Kernel/Time/Timeline.h"
#include "Kernel/Time/TimelineFactory.h"
#include "Kernel/Time/TimelineParameters.h"
#include "Scene/Components/ComponentSerializer.h"

namespace LevEngine
{
    TimelineComponent::TimelineComponent()
    {
        if (timeline == nullptr)
        {
            timeline = TimelineFactory::CreateTimeline(TimelineParameters{});
        }
        else
        {
            timeline->SetTimelineParameters(TimelineParameters{});
        }
    }

    void TimelineComponent::Init()
    {
        m_IsInited = true;

        // If timeline was playing in Edit mode, stop it
        if (IsPlaying())
            timeline->Stop();

        if (playOnInit)
        {
            timeline->Play();
        }
    }

    bool TimelineComponent::IsInitialized() const
    {
        return m_IsInited;
    }

    void TimelineComponent::ResetInit()
    {
        m_IsInited = false;
    }

    void TimelineComponent::Play() const
    {
        timeline->Play();
    }

    void TimelineComponent::Pause() const
    {
        timeline->Pause();
    }

    void TimelineComponent::Stop() const
    {
        timeline->Stop();
    }

    bool TimelineComponent::IsPlaying() const
    {
        return timeline->IsPlaying();
    }

    double TimelineComponent::GetDuration() const
    {
        return timeline->GetDuration();
    }

    void TimelineComponent::SetDuration(double duration)
    {
        timeline->SetDuration(duration);
    }

    double TimelineComponent::GetTimeScale() const
    {
        return timeline->GetTimeScale();
    }

    void TimelineComponent::SetTimeScale(double timeScale)
    {
        timeline->SetTimeScale(timeScale);
    }

    bool TimelineComponent::GetPlayOnInit() const
    {
        return this->playOnInit;
    }

    void TimelineComponent::SetPlayOnInit(bool playOnInit)
    {
        this->playOnInit = playOnInit;
    }

    bool TimelineComponent::GetIsLooping() const
    {
        return timeline->IsLooping();
    }

    void TimelineComponent::SetIsLooping(bool isLooping)
    {
        timeline->SetIsLooping(isLooping);
    }

    double TimelineComponent::GetElapsedTime() const
    {
        return timeline->GetTimeSinceStartup();
    }

    class TimelineComponentSerializer final : public ComponentSerializer<TimelineComponent, TimelineComponentSerializer>
    {
    protected:
        const char* GetKey() override { return "TimelineComponent"; } 

        void SerializeData(YAML::Emitter& out, const TimelineComponent& component) override
        {
            out << YAML::Key << "IsLooping" << YAML::Value << component.GetIsLooping();
            out << YAML::Key << "Duration" << YAML::Value << component.GetDuration();
            out << YAML::Key << "TimeScale" << YAML::Value << component.GetTimeScale();
            out << YAML::Key << "PlayOnInit" << YAML::Value << component.playOnInit;
        }
        
        void DeserializeData(YAML::Node& node, TimelineComponent& component) override
        {
            component.SetIsLooping(node["IsLooping"].as<bool>());
            component.SetDuration(node["Duration"].as<double>());
            component.SetTimeScale(node["TimeScale"].as<double>());
            component.playOnInit = node["PlayOnInit"].as<bool>();
            component.ResetInit();
        }
    };
}
