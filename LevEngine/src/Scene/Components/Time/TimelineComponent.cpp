#include "levpch.h"
#include "TimelineComponent.h"

#include "Kernel/Time/Timeline.h"
#include "Kernel/Time/TimelineFactory.h"
#include "Kernel/Time/TimelineParameters.h"
#include "Scene/Components/ComponentSerializer.h"

namespace LevEngine
{
    TimelineComponent::TimelineComponent() = default;

    void TimelineComponent::Init()
    {
        const auto parameters = CreateRef(TimelineParameters{playOnInit, isLooping, duration});
        
        if (timeline == nullptr)
        {
            timeline = TimelineFactory::CreateTimeline(parameters);
        }
        else
        {
            timeline->SetTimelineParameters(parameters);
        }
        
        m_IsInited = true;
    }

    bool TimelineComponent::GetIsLooping() const
    {
        return isLooping;
    }

    void TimelineComponent::SetIsLooping(bool isLooping)
    {
        this->isLooping = isLooping;

        if (timeline != nullptr)
        {
            timeline->SetIsLooping(isLooping);
        }
    }

    double TimelineComponent::GetElapsedTime() const
    {
        if (timeline != nullptr)
        {
            return timeline->GetTimeSinceStartup();
        }

        return 0.0f;
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
        if (timeline != nullptr)
        {
            timeline->Play();
        }
        else
        {
            Log::Warning("Timeline is not valid. Probably you are trying to play in Edit mode what is not allowed.");
        }
    }

    void TimelineComponent::Pause() const
    {
        if (timeline != nullptr)
        {
            timeline->Pause();
        }
    }

    void TimelineComponent::Stop() const
    {
        if (timeline != nullptr)
        {
            timeline->Stop();
        }
    }

    bool TimelineComponent::IsPlaying() const
    {
        return timeline != nullptr && timeline->IsPlaying();
    }

    double TimelineComponent::GetDuration() const
    {
        return duration;
    }

    void TimelineComponent::SetDuration(double duration)
    {
        this->duration = duration;
        
        if (timeline != nullptr)
        {
            timeline->SetDuration(duration);
        }
    }

    double TimelineComponent::GetTimeScale() const
    {
        return timeScale;
    }

    void TimelineComponent::SetTimeScale(double timeScale)
    {
        this->timeScale = timeScale;
        
        if (timeline != nullptr)
        {
            timeline->SetTimeScale(timeScale);
        }
    }

    bool TimelineComponent::GetPlayOnInit() const
    {
        return this->playOnInit;
    }

    void TimelineComponent::SetPlayOnInit(bool playOnInit)
    {
        this->playOnInit = playOnInit;
        
        if (timeline != nullptr)
        {
            timeline->SetPlayOnInit(playOnInit);
        }
    }

    class TimelineComponentSerializer final : public ComponentSerializer<TimelineComponent, TimelineComponentSerializer>
    {
    protected:
        const char* GetKey() override { return "TimelineComponent"; } 

        void SerializeData(YAML::Emitter& out, const TimelineComponent& component) override
        {
            out << YAML::Key << "IsLooping" << YAML::Value << component.isLooping;
            out << YAML::Key << "Duration" << YAML::Value << component.duration;
            out << YAML::Key << "TimeScale" << YAML::Value << component.timeScale;
            out << YAML::Key << "PlayOnInit" << YAML::Value << component.playOnInit;
        }
        
        void DeserializeData(YAML::Node& node, TimelineComponent& component) override
        {
            component.isLooping = node["IsLooping"].as<bool>();
            component.duration = node["Duration"].as<double>();
            component.timeScale = node["TimeScale"].as<double>();
            component.playOnInit = node["PlayOnInit"].as<bool>();
            component.ResetInit();
        }
    };
}
