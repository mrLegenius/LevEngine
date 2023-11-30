#include "levpch.h"
#include "Statistic.h"

namespace LevEngine
{
    Statistic::Statistic()
        : m_Total(0.0)
        , m_NumSamples(0.0)
        , m_Min(std::numeric_limits<double>::max())
        , m_Max(std::numeric_limits<double>::min())
    {
    }

    void Statistic::Reset()
    {
        m_Total = 0.0;
        m_NumSamples = 0.0;
        m_Min = std::numeric_limits<double>::max();
        m_Max = std::numeric_limits<double>::min();
    }

    void Statistic::Sample(const double value)
    {
        m_Total += value;
        m_NumSamples++;

        if (value > m_Max)
            m_Max = value;

        if (value < m_Min)
            m_Min = value;
    }

    double Statistic::GetNumSamples() const { return m_NumSamples; }

    double Statistic::GetAverage() const { return m_NumSamples > 0 ? m_Total / m_NumSamples : 0.0; }

    double Statistic::GetPerSecond() const
    {
        return m_Total > 0 ? m_NumSamples / m_Total * 1000.0 : 0.0;
    }

    double Statistic::GetMinValue() const { return m_Min; }

    double Statistic::GetMaxValue() const { return m_Max; }
}
