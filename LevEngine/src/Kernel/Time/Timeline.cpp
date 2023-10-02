#include "levpch.h"
#include "Timeline.h"

LevEngine::Timeline::Timeline() = default;

LevEngine::Timeline::Timeline(const TimelineParameters&& timelineParameters)
{
    m_TimelineParameters = timelineParameters;
}

double LevEngine::Timeline::GetTimeScale() const
{
    return m_TimelineParameters.timeScale;
}

double LevEngine::Timeline::GetTimeSinceStartup() const
{
    return m_TimeElapsed;
}

void LevEngine::Timeline::Play()
{
    LEV_ASSERT(IsValid());
    
    if (!IsLooping() && IsMaxDurationReached())
    {
        m_TimeElapsed = 0.0;
    }

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
                m_TimeElapsed -= m_TimelineParameters.duration * Math::Sign(GetTimeScale());
            }
            while (IsMaxDurationReached());
        }
        else
        {
            m_TimeElapsed = m_TimelineParameters.duration;
            Pause();
        }
    }
}

bool LevEngine::Timeline::IsPlaying() const
{
    return m_IsPlaying;
}

bool LevEngine::Timeline::IsLooping() const
{
    return m_TimelineParameters.isLooping;
}

void LevEngine::Timeline::SetDuration(double duration)
{
    m_TimelineParameters.duration = duration;
}

void LevEngine::Timeline::SetIsLooping(bool isLooping)
{
    m_TimelineParameters.isLooping = isLooping;
}

void LevEngine::Timeline::SetTimeScale(const double timeScale)
{
    m_TimelineParameters.timeScale = timeScale;
}

void LevEngine::Timeline::SetTimelineParameters(const TimelineParameters& timelineParameters)
{
    m_TimelineParameters = timelineParameters;
}

double LevEngine::Timeline::GetDuration() const
{
    return m_TimelineParameters.duration;
}

bool LevEngine::Timeline::IsValid() const
{
    return m_TimelineParameters.duration >= 0 || !m_TimelineParameters.isLooping;
}
