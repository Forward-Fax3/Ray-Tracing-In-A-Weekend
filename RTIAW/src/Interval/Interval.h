#pragma once
#include "Core.h"


namespace RTW
{
	class Interval
	{
	public:
		inline Interval() : m_Min(DoubleInf), m_Max(-DoubleInf) {}
		inline Interval(const double min, const double max) : m_Min(min), m_Max(max) {}
		inline Interval(const Interval& old) : m_Min(old.m_Min), m_Max(old.m_Max) {}
		inline Interval(Interval&& old) : m_Min(old.m_Min), m_Max(old.m_Max)
			{ old.m_Min = DoubleInf; old.m_Max = -DoubleInf; }

		inline double Size() const { return m_Max - m_Min; }

		inline bool Contains(const double x) const { return m_Min <= x && x <= m_Max; }
		inline bool Contains(const Interval& other) const
			{ return m_Min <= other.m_Min && other.m_Max <= m_Max; }

		inline bool Surrounds(const double x) const { return m_Min < x && x < m_Max; }
		inline bool Surrounds(const Interval& other) const
			{ return m_Min < other.m_Min && other.m_Max < m_Max; }

		inline double Clamp(const double x) const { return glm::clamp(x, m_Min, m_Max); }
		inline Colour Clamp(const Colour& x) const { return glm::clamp(x, Vec3(m_Min), Vec3(m_Max)); }

		inline double GetMax() const { return m_Max; }
		inline double GetMin() const { return m_Min; }
		inline void SetMax(const double max) { m_Max = max; }
		inline void SetMin(const double min) { m_Min = min; }

		static const Interval Empty;
		static const Interval Univers;

	private:
		double m_Min;
		double m_Max;
	};
}