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
        void SetTimeScale(double timeScale);
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

    private:
        [[nodiscard]] bool IsMaxDurationReached() const
        {
            return m_TimeElapsed + Math::FloatEpsilon > m_TimelineParameters->duration;
        }
        
        double m_TimeScale;
        double m_TimeElapsed;
        bool m_IsPlaying;
        Ref<TimelineParameters> m_TimelineParameters;
    };
}


