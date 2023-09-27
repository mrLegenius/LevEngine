#include "levpch.h"
#include "Timeline.h"

LevEngine::Timeline::Timeline(Ref<TimelineParameters> timelineParameters)
{
    LEV_ASSERT(timelineParameters->duration > -Math::FloatEpsilon || !timelineParameters->isLooping);
    
    m_TimelineParameters = timelineParameters;

    if (m_TimelineParameters->startPlayingOnStart)
    {
        Play();
    }
}

double LevEngine::Timeline::GetTimeScale() const
{
    return m_TimeScale;
}

void LevEngine::Timeline::SetTimeScale(const double timeScale)
{
    m_TimeScale = timeScale;
}

double LevEngine::Timeline::GetTimeSinceStartup() const
{
    return m_TimeElapsed;
}

void LevEngine::Timeline::Play()
{
    m_IsPlaying = true;
}

void LevEngine::Timeline::Pause()
{
    m_IsPlaying = false;
}

void LevEngine::Timeline::Stop()
{
    m_IsPlaying = false;
    m_TimeElapsed = 0.0;
}

void LevEngine::Timeline::OnUpdate(float deltaTime)
{
    if (IsPlaying())
    {
        m_TimeElapsed += static_cast<double>(deltaTime) * GetTimeScale();

        if (!IsMaxDurationReached())
        {
            return;
        }

        if (IsLooping())
        {
            do
            {
                m_TimeElapsed -= m_TimelineParameters->duration;
            }
            while (IsMaxDurationReached());
        }
        else
        {
            m_TimeElapsed = m_TimelineParameters->duration;
            Pause();
        }
    }
}

bool LevEngine::Timeline::IsPlaying() const
{
    return m_IsPlaying;
}
