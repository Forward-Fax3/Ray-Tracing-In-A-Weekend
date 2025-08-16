#pragma once
#include "Core.h"


namespace RTW
{
	class Interval
	{
	public:
		inline Interval() : m_Min(doubleInf), m_Max(-doubleInf) {}
		inline Interval(const double min, const double max) : m_Min(min), m_Max(max) {}
		inline Interval(const Interval& old) : m_Min(old.m_Min), m_Max(old.m_Max) {}
		inline Interval(Interval&& old) noexcept : m_Min(old.m_Min), m_Max(old.m_Max)
			{ old.m_MinMax.data = Empty.m_MinMax.data; }
		inline Interval(const Interval& a, const Interval& b)
			: m_Min((a.m_Min <= b.m_Min) ? a.m_Min : b.m_Min), m_Max((a.m_Max >= b.m_Max) ? a.m_Max : b.m_Max) {}

		~Interval() = default;

		inline Interval& operator=(const Interval& original) { this->m_Min = original.m_Min; this->m_Max = original.m_Max; return *this; }

		inline double Size() const { return m_Max - m_Min; }

		inline bool Contains(const double x) const { return m_Min <= x && x <= m_Max; }
		inline bool Contains(const Interval& other) const
			{ return m_Min <= other.m_Min && other.m_Max <= m_Max; }
		inline bool Contains(const UV& uv) const
			{ return Contains(uv.x) && Contains(uv.y); }

		inline bool Surrounds(const double x) const { return m_Min < x && x < m_Max; }
		inline bool Surrounds(const Interval& other) const
			{ return m_Min < other.m_Min && other.m_Max < m_Max; }

		inline double Clamp(const double x)  const { return glm::clamp(x, m_Min, m_Max); }
		inline UV     Clamp(const UV& uv)    const { return glm::clamp(uv, UV(m_Min), UV(m_Max)); }
		inline Colour Clamp(const Colour& x) const { return glm::clamp(x, Vec3(m_Min), Vec3(m_Max)); }

		inline void Expand(double dalta)
			{ this->m_Min -= dalta * 0.5; this->m_Max += dalta * 0.5; }
		inline void Expand(double minIncrease, double maxIncrease)
			{ this->m_Min -= minIncrease; this->m_Max += maxIncrease; }
		inline void Expand(const Interval& newInterval)
			{ m_Min = glm::min(m_Min, newInterval.m_Min); m_Max = glm::max(m_Max, newInterval.m_Max); }

		inline double GetMax() const { return m_Max; }
		inline double GetMin() const { return m_Min; }
		inline const glm::dvec2& GetAsVector() const { return m_MinMax; }

		inline void SetMax(const double max) { m_Max = max; }
		inline void SetMin(const double min) { m_Min = min; }
		inline void SetMinMax(const double min, const double max) { m_Min = min; m_Max = max; }
		inline void SetMinMax(const __m128d minMax) { m_MinMax.data = minMax; }

		static const Interval Empty;
		static const Interval Univers;

	private:
		union
		{
			struct
			{
				double m_Min;
				double m_Max;
			};
			glm::dvec2 m_MinMax{};
		};
	};
}