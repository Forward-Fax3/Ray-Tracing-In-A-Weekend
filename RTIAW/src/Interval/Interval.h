#pragma once
#include "Core.h"


namespace RTW
{
	class Interval
	{
	public:
		inline Interval() : m_Min(doubleInf), m_Max(-doubleInf) {}
		inline Interval(const double min, const double max) : m_Min(min), m_Max(max) {}
		explicit inline Interval(const __m128d& minMax) : Interval() { m_MinMax.data = minMax; }
		inline Interval(const Interval& old) : m_Min(old.m_Min), m_Max(old.m_Max) {}
		inline Interval(Interval&& old) noexcept : m_Min(old.m_Min), m_Max(old.m_Max)
			{ old.m_MinMax.data = Empty.m_MinMax.data; }
		inline Interval(const Interval& a, const Interval& b)
			: m_Min(glm::min(a.m_Min, b.m_Min)), m_Max(glm::max(a.m_Max, b.m_Max)) {}

		inline ~Interval()
			{ this->m_MinMax.data = Empty.m_MinMax.data; }

		inline Interval& operator=(const Interval& original) { this->m_MinMax.data = original.m_MinMax.data; return *this; }
		inline Interval& operator=(Interval&& old) noexcept { this->m_MinMax.data = old.m_MinMax.data; old.m_MinMax.data = Empty.m_MinMax.data; return *this; }

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

		inline void Expand(double delta)
			{ this->m_Min -= delta * 0.5; this->m_Max += delta * 0.5; }
		inline void Expand(double minIncrease, double maxIncrease)
			{ this->m_Min -= minIncrease; this->m_Max += maxIncrease; }
		inline void Expand(const Interval& newInterval)
			{ m_Min = glm::min(m_Min, newInterval.m_Min); m_Max = glm::max(m_Max, newInterval.m_Max); }

		inline void Shrink(double delta)
			{ this->m_Min += delta * 0.5; this->m_Max -= delta * 0.5; }
		inline void Shrink(double minShrink, double maxShrink)
			{ this->m_Min -= minShrink; this->m_Max += maxShrink; }
		inline void Shrink(const Interval& newInterval)
		{
			const __m128d m128_InvertValue = _mm_set_pd(-1.0, 1.0);
			glm::dvec2 invThisValues{};
			glm::dvec2 invNewValues{};
			invThisValues.data = _mm_mul_pd(m_MinMax.data, m128_InvertValue);
			invNewValues.data = _mm_mul_pd(newInterval.GetAsVector().data, m128_InvertValue);

			m_MinMax.data = _mm_mul_pd(m128_InvertValue, _mm_max_pd(invThisValues.data, invNewValues.data));
		}
		inline void Shrink(const __m128d& newInterval)
		{
			const __m128d m128_InvertValue = _mm_set_pd(-1.0, 1.0);
			glm::dvec2 invThisValues{};
			glm::dvec2 invNewValues{};
			invThisValues.data = _mm_mul_pd(m_MinMax.data, m128_InvertValue);
			invNewValues.data = _mm_mul_pd(newInterval, m128_InvertValue);

			m_MinMax.data = _mm_mul_pd(m128_InvertValue, _mm_max_pd(invThisValues.data, invNewValues.data));
		}

		inline bool IsEqualOrBigger(const Interval& other) const
			{ return this->GetDelta() >= other.GetDelta(); }

		inline double GetDelta() const { return m_Max - m_Min; }

		inline double GetMax() const { return m_Max; }
		inline double GetMin() const { return m_Min; }
		inline const glm::dvec2& GetAsVector() const { return m_MinMax; }

		inline void SetMin(const double min) { m_Min = min; }
		inline void SetMax(const double max) { m_Max = max; }
		inline void SetMinMax(const double min, const double max) { m_Min = min; m_Max = max; }
		inline void SetMinMax(const glm::dvec2 minMax) { m_MinMax.data = minMax.data; }
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
			Vec2 m_MinMax{};
		};
	};
}