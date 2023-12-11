#pragma once

namespace LevEngine
{
    struct Statistic
    {
        Statistic();
        
        void Reset();

        void Sample(double value);

        double GetNumSamples() const;
        double GetAverage() const;
        double GetPerSecond() const;
        double GetMinValue() const;
        double GetMaxValue() const;

    private:
        double m_Total;
        double m_NumSamples;
        double m_Min;
        double m_Max;
    };
}
