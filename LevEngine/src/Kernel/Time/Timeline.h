#pragma once
#include "Timestep.h"
#include "Math/Math.h"

namespace LevEngine
{
    class Timeline
    {
    public:
        Timeline(bool startPlaying = true, bool isLooping = false, double loopDuration = 0.0);

        [[nodiscard]] double GetTimeScale() const;
        void SetTimeScale(double timeScale);
        [[nodiscard]] double GetTimeSinceStartup() const;

        void Play();
        void Pause();
        void Stop();
        
        [[nodiscard]] bool IsLooping() const
        {
            return m_IsLooping;
        }
        
        void OnUpdate(float deltaTime);
        [[nodiscard]] bool IsPlaying() const;

    private:
        [[nodiscard]] bool IsMaxDurationReached() const
        {
            return m_TimeElapsed + Math::FloatEpsilon > m_Duration;
        }
        
        double m_TimeScale;
        double m_TimeElapsed;
        bool m_IsPlaying;
        bool m_IsLooping;

        // If m_IsLooping, then equals loop duration, equals total play duration otherwise
        // If !m_IsLooping && negative, then duration is infinite
        double m_Duration; 
    };
}


