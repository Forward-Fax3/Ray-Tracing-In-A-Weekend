#include "Core.h"
#include "Interval.h"
#include "Ray.h"
#include "AxisAliagnedBoundingBoxes.h"

#include <immintrin.h>

#define SIMD 1


namespace RTW
{
	AABB::AxisAliagnedBoundingBoxes(const Interval& interval)
		: m_X(interval), m_Y(interval), m_Z(interval) { s_NumberofBoundingBoxes++; m_LongestAxis = LongestAxisSetter(); }

	AABB::AxisAliagnedBoundingBoxes(const Interval& x, const Interval& y, const Interval& z)
		: m_X(x), m_Y(y), m_Z(z) { s_NumberofBoundingBoxes++; m_LongestAxis = LongestAxisSetter(); }

	AABB::AxisAliagnedBoundingBoxes(const Point& a, const Point& b)
	{
		s_NumberofBoundingBoxes++;
		m_X = (a.x <= b.x) ? Interval(a.x, b.x) : Interval(b.x, a.x);
		m_Y = (a.y <= b.y) ? Interval(a.y, b.y) : Interval(b.y, a.y);
		m_Z = (a.z <= b.z) ? Interval(a.z, b.z) : Interval(b.z, a.z);
		m_LongestAxis = LongestAxisSetter();
	}

	AABB::AxisAliagnedBoundingBoxes(const AABB& box0, const AABB& box1)
		: m_X(box0.m_X, box1.m_X), m_Y(box0.m_Y, box1.m_Y), m_Z(box0.m_Z, box1.m_Z) { s_NumberofBoundingBoxes++; m_LongestAxis = LongestAxisSetter(); }

	const Interval& AABB::GetAxisInterval(const Axis& axis) const
	{
		switch (axis)
		{
		case Axis::x:
			return m_X;
		case Axis::y:
			return m_Y;
		case Axis::z:
			return m_Z;
		default:
			return Interval::Univers;
		}
	}

	bool AABB::IsHit(const Ray& ray, Interval rayT) const
	{
		for (Axis axis = Axis::x; axis <= Axis::z; axis++)
		{
			const Interval& axisBounds = GetAxisInterval(axis);
			const double rAxisDirection = 1.0 / ray.direction()[+axis];

			glm::dvec2 t = (axisBounds.GetAsVector() - ray.origin()[+axis]) * rAxisDirection;

			if (t.x >= t.y)
				std::swap(t.x, t.y);

#if defined(RTW_AVX2) | defined(RTW_AVX512) & (SIMD == 1)
			__m128d invertValue = { 1.0, -1.0 };
			__m128d m128_T = _mm_mul_pd(invertValue, t.data);
			__m128d m128_RayT = _mm_mul_pd(invertValue, rayT.GetAsVector().data);

#ifdef RTW_AVX512
			__mmask8 bitMask = _mm_cmp_pd_mask(m128_T, m128_RayT, _CMP_GT_OS);
			m128_RayT = _mm_mask_load_pd(rayT.GetAsVector().data, bitMask, &t);
			_mm_store_pd(const_cast<double*>(&rayT.GetAsVector().x), m128_RayT);

			if (m128_RayT.m128d_f64[0] >= m128_RayT.m128d_f64[1])
				return false;

#else // ifdef RTW_AVX512
			__m128d bitMask = _mm_cmpgt_pd(m128_T, m128_RayT);
			m128_RayT = _mm_blendv_pd(rayT.GetAsVector().data, t.data, bitMask);
			_mm_store_pd(const_cast<double*>(&rayT.GetAsVector().x), m128_RayT);

			if (m128_RayT.m128d_f64[0] >= m128_RayT.m128d_f64[1])
				return false;

#endif // ifdef RTW_AVX512
#else // if defined(RTW_AVX2) | defined(RTW_AVX512) & (SIMD == 1)
			if (t.x > rayT.GetMin()) 
				rayT.SetMin(t.x);
			if (t.y < rayT.GetMax())
				rayT.SetMax(t.y);

			if (rayT.GetMin() >= rayT.GetMax())
				return false;
#endif // if defined(RTW_AVX2) | defined(RTW_AVX512) & (SIMD == 1)
		}
		return true;
	}

	RTW::AABB::Axis AxisAliagnedBoundingBoxes::LongestAxisSetter() const
	{
		return (m_X.Size() > m_Y.Size()) ?
			((m_X.Size() > m_Z.Size()) ? AABB::Axis::x : AABB::Axis::z) :
			((m_Y.Size() > m_Z.Size()) ? AABB::Axis::y : AABB::Axis::z);
	}

	size_t AxisAliagnedBoundingBoxes::s_NumberofBoundingBoxes = 0;

	const AABB AABB::empty = { Interval::Empty };
	const AABB AABB::univers = { Interval::Univers };
}
