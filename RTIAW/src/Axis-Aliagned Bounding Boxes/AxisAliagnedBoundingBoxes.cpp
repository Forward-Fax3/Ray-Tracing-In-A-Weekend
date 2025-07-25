#include "Core.h"
#include "Interval.h"
#include "Ray.h"
#include "AxisAliagnedBoundingBoxes.h"

#include <immintrin.h>

#define SIMD 1
#define AVX512 1


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
		const __m128d m128_InvertValue = { 1.0, -1.0 };
#if defined(RTW_AVX512) & (AVX512 == 1)
		const __mmask8 loadMask = 0b00111111;
		__m512d m512_AxisBounds = _mm512_maskz_load_pd(loadMask, &m_X);
		const Vec3& rayOrigin = ray.origin();
		__m512d m512_RayOrigin = { rayOrigin.x, rayOrigin.x, rayOrigin.y, rayOrigin.y, rayOrigin.z, rayOrigin.z, 0.0, 0.0 };
		const Vec3& rayDirection = ray.direction();
		__m512d m512_RayDirection = { rayDirection.x, rayDirection.x, rayDirection.y, rayDirection.y, rayDirection.z, rayDirection.z, 0.0, 0.0 };
		__m512d m512_InvRayDiraction = _mm512_div_pd(_mm512_set1_pd(1.0), m512_RayDirection);

		__m512d m512_T = _mm512_mul_pd(_mm512_sub_pd(m512_AxisBounds, m512_RayOrigin), m512_InvRayDiraction);

#if __clang__
		for (size_t i = 0; i < 6; i += 2)
			if (m512_T[i] > m512_T[i + 1])
			{
				double temp = m512_T[i];
				m512_T[i] = m512_T[i + 1];
				m512_T[i + 1] = temp;
			}
#elif _MSC_VER
		__m256d m512_TMin = { m512_T.m512d_f64[0], m512_T.m512d_f64[2], m512_T.m512d_f64[4], m512_T.m512d_f64[6] };
		__m256d m512_TMax = { m512_T.m512d_f64[1], m512_T.m512d_f64[3], m512_T.m512d_f64[5], m512_T.m512d_f64[7] };

		__mmask8 TBaseSwapMask = _mm256_cmp_pd_mask(m512_TMin, m512_TMax, _CMP_GT_OS);
		uint32_t setSwapMaskBitMask = static_cast<uint32_t>(0b01010101);
		__mmask8 TSwapMask = static_cast<__mmask8>(_pdep_u32(static_cast<uint32_t>(TBaseSwapMask), setSwapMaskBitMask));
		setSwapMaskBitMask <<= 1;
		TSwapMask |= static_cast<__mmask8>(_pdep_u32(static_cast<uint32_t>(TBaseSwapMask), setSwapMaskBitMask));

		m512_T = _mm512_mask_permute_pd(m512_T, TSwapMask, m512_T, 0b01010101);
#else
#error only supports clang and msc
#endif

		__m512d m512_RayT = _mm512_maskz_broadcast_f64x2(loadMask, rayT.GetAsVector().data);

		const __m512d m512_InvertValue = _mm512_maskz_broadcast_f64x2(loadMask, m128_InvertValue);

		__m512d m512_AltInvT = _mm512_mul_pd(m512_T, m512_InvertValue);
		__m512d m512_AltInvRayT = _mm512_mul_pd(m512_RayT, m512_InvertValue);

		__m512d m512_MaxMinT = _mm512_mul_pd(_mm512_max_pd(m512_AltInvRayT, m512_AltInvT), m512_InvertValue);

#if __clang__
		for (size_t i = 0; i < 6; i += 2)
			if (m512_MaxMinT[i] >= m512_MaxMinT[i + 1])
				return false;
#elif _MSC_VER
		for (size_t i = 0; i < 6; i += 2)
			if (m512_MaxMinT.m512d_f64[i] >= m512_MaxMinT.m512d_f64[i + 1])
				return false;
#endif

#else // RTW_AVX512
		for (Axis axis = Axis::x; axis <= Axis::z; axis++)
		{
			const Interval& axisBounds = GetAxisInterval(axis);
			const double rAxisDirection = 1.0 / ray.direction()[+axis];

			glm::dvec2 t = (axisBounds.GetAsVector() - ray.origin()[+axis]) * rAxisDirection;

			if (t.x > t.y)
				std::swap(t.x, t.y);

#if defined(RTW_AVX2) | defined(RTW_AVX512) & (SIMD == 1)
			__m128d m128_T = _mm_mul_pd(m128_InvertValue, t.data);
			__m128d m128_RayT = _mm_mul_pd(m128_InvertValue, rayT.GetAsVector().data);

#ifdef RTW_AVX512
			__mmask8 bitMask = _mm_cmp_pd_mask(m128_T, m128_RayT, _CMP_GT_OS);
			m128_RayT = _mm_mask_load_pd(rayT.GetAsVector().data, bitMask, &t);
			_mm_store_pd(const_cast<double*>(&rayT.GetAsVector().x), m128_RayT);
#if __clang__
			if (m128_RayT[0] >= m128_RayT[1])
				return false;
#elif _MSC_VER
			if (m128_RayT.m128d_f64[0] >= m128_RayT.m128d_f64[1])
				return false;
#endif

#else // RTW_AVX512
			__m128d bitMask = _mm_cmpgt_pd(m128_T, m128_RayT);
			m128_RayT = _mm_blendv_pd(rayT.GetAsVector().data, t.data, bitMask);
			_mm_store_pd(const_cast<double*>(&rayT.GetAsVector().x), m128_RayT);

			if (m128_RayT.m128d_f64[0] >= m128_RayT.m128d_f64[1])
				return false;

#endif // RTW_AVX512
#else // defined(RTW_AVX2) | defined(RTW_AVX512) & (SIMD == 1)
			if (t.x > rayT.GetMin()) 
				rayT.SetMin(t.x);
			if (t.y < rayT.GetMax())
				rayT.SetMax(t.y);

			if (rayT.GetMin() >= rayT.GetMax())
				return false;
#endif // defined(RTW_AVX2) | defined(RTW_AVX512) & (SIMD == 1)
		}
#endif // RTW_AVX512
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
