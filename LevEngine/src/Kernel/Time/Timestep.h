#pragma once

namespace LevEngine
{
	class Timestep
	{
	public:
		explicit Timestep(const double time = 0.0) : m_Time(time) { }

		[[nodiscard]] float GetSeconds() const { return static_cast<float>(m_Time); }
		[[nodiscard]] float GetMilliseconds() const { return static_cast<float>(m_Time) * 1000.0f; }
		Timestep operator*(const Timestep& rhs) const;
		Timestep operator*(double rhs) const;
		Timestep& operator+=(const Timestep& rhs);

	private:
		double m_Time;
	};

	inline Timestep Timestep::operator*(const Timestep& rhs) const
	{
		return Timestep(m_Time * rhs.m_Time);
	}

	inline Timestep Timestep::operator*(const double rhs) const
	{
		return Timestep(m_Time * rhs);
	}

	inline Timestep& Timestep::operator+=(const Timestep& rhs)
	{
		m_Time += rhs.m_Time;
		return *this;
	}
}
