#include "Core.h"
#include "Interval.h"
#include "Ray.h"
#include "AxisAliagnedBoundingBoxes.h"

#include <immintrin.h>
#include <atomic>


namespace RTW
{
	std::atomic<size_t> AxisAliagnedBoundingBoxes::s_NumberOfBoundingBoxes = 0;

	const AABB AABB::empty(Interval::Empty);
	const AABB AABB::univers(Interval::Univers);

	AABB::AxisAliagnedBoundingBoxes(const Interval& interval)
		: m_X(interval), m_Y(interval), m_Z(interval) { s_NumberOfBoundingBoxes++; MiniumPadding(); LongestAxisSetter(); }

	AABB::AxisAliagnedBoundingBoxes(const Interval& x, const Interval& y, const Interval& z)
		: m_X(x), m_Y(y), m_Z(z) { s_NumberOfBoundingBoxes++; MiniumPadding(); LongestAxisSetter(); }

	AABB::AxisAliagnedBoundingBoxes(const Point& a, const Point& b)
	{
		s_NumberOfBoundingBoxes++;
		m_X = (a.x <= b.x) ? Interval(a.x, b.x) : Interval(b.x, a.x);
		m_Y = (a.y <= b.y) ? Interval(a.y, b.y) : Interval(b.y, a.y);
		m_Z = (a.z <= b.z) ? Interval(a.z, b.z) : Interval(b.z, a.z);
		MiniumPadding();
		LongestAxisSetter();
	}

	AxisAliagnedBoundingBoxes::AxisAliagnedBoundingBoxes(const AxisAliagnedBoundingBoxes& otherAABB)
		: m_X(otherAABB.m_X), m_Y(otherAABB.m_Y), m_Z(otherAABB.m_Z), m_LongestAxis(otherAABB.m_LongestAxis)
	{
		s_NumberOfBoundingBoxes++;
	}

	AABB::AxisAliagnedBoundingBoxes(const AxisAliagnedBoundingBoxes& box0, const AxisAliagnedBoundingBoxes& box1)
		: m_X(box0.m_X, box1.m_X), m_Y(box0.m_Y, box1.m_Y), m_Z(box0.m_Z, box1.m_Z) { s_NumberOfBoundingBoxes++; MiniumPadding(); LongestAxisSetter(); }

	const Interval& AABB::GetAxisInterval(const Axis& axis) const
	{
		switch (axis)
		{
		using enum Axis;
		case x:
			return m_X;
		case y:
			return m_Y;
		case z:
			return m_Z;
		case none:
		default:
			return Interval::Univers;
		}
	}

	bool AxisAliagnedBoundingBoxes::IsBigger(const AxisAliagnedBoundingBoxes& otherAABB) const
	{
		bool returnBool = true;
		returnBool &= this->m_X.Contains(otherAABB.m_X);
		returnBool &= this->m_Y.Contains(otherAABB.m_Y);
		returnBool &= this->m_Z.Contains(otherAABB.m_Z);
		return returnBool;
	}

#define SIMD 0

	void AxisAliagnedBoundingBoxes::Expand(const AxisAliagnedBoundingBoxes& newAABB)
	{
#if (SIMD == 1)
		__m128d mins = { m_X.GetMin(), m_Y.GetMin() };
		__m128d maxs = { m_X.GetMax(), m_Y.GetMax() };
		__m128d newMins = { newAABB.m_X.GetMin(), newAABB.m_Y.GetMin() };
		__m128d newMaxs = { newAABB.m_X.GetMax(), newAABB.m_Y.GetMax() };
		mins = _mm_min_pd(mins, newMins);
		maxs = _mm_max_pd(maxs, newMaxs);

#if __clang__
		m_X.SetMin(mins[0]);
		m_Y.SetMin(mins[1]);

		m_X.SetMax(maxs[0]);
		m_Y.SetMax(maxs[1]);
#elif _MSC_VER
		m_X.SetMin(mins.m128d_f64[0]);
		m_Y.SetMin(mins.m128d_f64[1]);

		m_X.SetMax(maxs.m128d_f64[0]);
		m_Y.SetMax(maxs.m128d_f64[1]);
#endif
#else
		m_X.Expand(newAABB.m_X);
		m_Y.Expand(newAABB.m_Y);
#endif
		m_Z.Expand(newAABB.m_Z);
		MiniumPadding();
		LongestAxisSetter();
	}

	void AxisAliagnedBoundingBoxes::LongestAxisSetter()
	{
		if (m_X.Size() > m_Y.Size())
			m_LongestAxis = (m_X.Size() > m_Z.Size()) ? AABB::Axis::x : AABB::Axis::z;
		else
			m_LongestAxis = (m_Y.Size() > m_Z.Size()) ? AABB::Axis::y : AABB::Axis::z;
	}

	void AxisAliagnedBoundingBoxes::MiniumPadding()
	{
		static constexpr double minium = 1e-4;
		if (m_X.Size() < minium)
			m_X.Expand(minium);
		if (m_Y.Size() < minium)
			m_Y.Expand(minium);
		if (m_Z.Size() < minium)
			m_Z.Expand(minium);
	}
}
