#pragma once
#include "TimelineParameters.h"
#include "Math/Math.h"

namespace LevEngine
{
    class Timeline
    {
    public:
        Timeline(Ref<TimelineParameters> timelineParameters);
        
        [[nodiscard]] double GetTimeScale() const;
        [[nodiscard]] double GetTimeSinceStartup() const;

        void Play();
        void Pause();
        void Stop();
        
        [[nodiscard]] bool IsLooping() const
        {
            return m_TimelineParameters->isLooping;
        }
        
        void OnUpdate(float deltaTime);
        [[nodiscard]] bool IsPlaying() const;
        void SetDuration(double duration);
        void SetIsLooping(bool isLooping);
        void SetPlayOnInit(bool playOnInit);
        void SetTimeScale(double timeScale);
        void SetTimelineParameters(Ref<TimelineParameters> timelineParameters);

    private:
        [[nodiscard]] bool IsMaxDurationReached() const
        {
            // If TimeScale >= 0
            if (GetTimeScale() > -Math::FloatEpsilon)
            {
                return m_TimeElapsed + Math::FloatEpsilon > m_TimelineParameters->duration;
            }

            // If TimeScale < 0
            return m_TimeElapsed < Math::FloatEpsilon;
        }
        
        double m_TimeElapsed{};
        bool m_IsPlaying;
        Ref<TimelineParameters> m_TimelineParameters;
    };
}


