#pragma once
#include "TimelineParameters.h"
#include "Math/Math.h"

namespace LevEngine
{
    class Timeline
    {
    public:
        Timeline();
        Timeline(const TimelineParameters&& timelineParameters);
        
        [[nodiscard]] double GetTimeScale() const;
        [[nodiscard]] double GetTimeSinceStartup() const;

        void Play();
        void Pause();
        void Stop();

        void OnUpdate(float deltaTime);
        [[nodiscard]] bool IsPlaying() const;
        [[nodiscard]] bool IsLooping() const;
        void SetDuration(double duration);
        void SetIsLooping(bool isLooping);
        void SetTimeScale(double timeScale);
        void SetTimelineParameters(const TimelineParameters& timelineParameters);
        double GetDuration() const;

    private:
        [[nodiscard]] bool IsMaxDurationReached() const
        {
            if (GetTimeScale() >= 0)
            {
                return m_TimeElapsed + Math::FloatEpsilon > m_TimelineParameters.duration;
            }

            return m_TimeElapsed <= 0;
        }

        bool IsValid() const;
        
        double m_TimeElapsed{};
        bool m_IsPlaying{};
        TimelineParameters m_TimelineParameters;
    };
}


