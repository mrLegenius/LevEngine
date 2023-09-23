#pragma once

namespace LevEngine
{
	class Timestep
	{
	public:
		explicit Timestep(const float time = 0.0f) : m_Time(time) { }
		explicit Timestep(const double time = 0.0) : m_Time(time) { }

		explicit operator float() const { return m_Time; }

		[[nodiscard]] float GetSeconds() const { return m_Time; }
		[[nodiscard]] float GetMilliseconds() const { return m_Time * 1000.0f; }
		Timestep operator*(const Timestep& rhs) const;
		Timestep operator*(double rhs) const;
		Timestep& operator+=(const Timestep& rhs);

	private:
		float m_Time;
	};

	inline Timestep Timestep::operator*(const Timestep& rhs) const
	{
		return Timestep(m_Time * rhs.m_Time);
	}

	inline Timestep Timestep::operator*(double rhs) const
	{
		return Timestep(static_cast<double>(m_Time) * rhs);
	}

	inline Timestep& Timestep::operator+=(const Timestep& rhs)
	{
		m_Time += rhs.m_Time;
		return *this;
	}
}
