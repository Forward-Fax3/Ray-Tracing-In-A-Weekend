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
		inline Interval(const Interval& a, const Interval& b)
			: m_Min((a.m_Min <= b.m_Min) ? a.m_Min : b.m_Min), m_Max((a.m_Max >= b.m_Max) ? a.m_Max : b.m_Max) {}

		inline Interval operator=(const Interval& original) { this->m_Min = original.m_Min; this->m_Max = original.m_Max; return *this; }

		inline double Size() const { return m_Max - m_Min; }

		inline bool Contains(const double x) const { return m_Min <= x && x <= m_Max; }
		inline bool Contains(const Interval& other) const
			{ return m_Min <= other.m_Min && other.m_Max <= m_Max; }

		inline bool Surrounds(const double x) const { return m_Min < x && x < m_Max; }
		inline bool Surrounds(const Interval& other) const
			{ return m_Min < other.m_Min && other.m_Max < m_Max; }

		inline double Clamp(const double x)  const { return glm::clamp(x, m_Min, m_Max); }
		inline UV     Clamp(const UV& uv)     const { return glm::clamp(uv, glm::dvec2(m_Min), glm::dvec2(m_Max)); }
		inline Colour Clamp(const Colour& x) const { return glm::clamp(x, Vec3(m_Min), Vec3(m_Max)); }

		[[nodiscard]] inline Interval Expand(double dalta) const
			{ return { m_Min + dalta * 0.5, m_Max + dalta * 0.5 }; }
		[[nodiscard]] inline Interval Expand(double minIncrease, double maxIncrease) const
			{ return { m_Min + minIncrease, m_Max + maxIncrease }; }

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