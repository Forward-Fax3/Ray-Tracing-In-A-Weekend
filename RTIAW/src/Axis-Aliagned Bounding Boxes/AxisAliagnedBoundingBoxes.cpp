#include "Core.h"
#include "Interval.h"
#include "Ray.h"
#include "AxisAliagnedBoundingBoxes.h"

#include <immintrin.h>
#include <atomic>

#define SIMD 1
#define AVX512 0 // twice as slow at the moment and I cant think of any way to make it faster


namespace RTW
{
	std::atomic<size_t> AxisAliagnedBoundingBoxe::s_NumberofBoundingBoxes = 0;

	const AABB AABB::empty = { Interval::Empty };
	const AABB AABB::univers = { Interval::Univers };

#if defined(RTW_AVX512) & (AVX512 == 1)
	static bool CheckOrderedPairs0To5(__m512d vec) {
		// Shuffle vec so that we get the comparison pairs:
		// Pair vec[0] with vec[1], vec[2] with vec[3], vec[4] with vec[5], vec[6], vec[7]
		// [0 1 2 3 4 5 6 7]
		// [1 0 3 2 5 4 7 6]
		const __m512i m512_SwapBitMap = _mm512_set_epi64(6, 7, 4, 5, 2, 3, 0, 1);
		__m512d shuffled = _mm512_permutexvar_pd(m512_SwapBitMap, vec);

		// Perform less-than comparisons on relevant pairs
		__mmask8 mask = _mm512_cmp_pd_mask(vec, shuffled, _CMP_LT_OQ);

		// We only care about bits 0, 2, 4 (corresponding to comparisons 0<1, 2<3, 4<5)
		// So we mask those out and check if all are set
		const __mmask8 relevant_mask = 0b00010101; // bits 0, 2, 4

		return (mask & relevant_mask) == relevant_mask;
	}
#endif

	AABB::AxisAliagnedBoundingBoxe(const Interval& interval)
		: m_X(interval), m_Y(interval), m_Z(interval) { s_NumberofBoundingBoxes++; MiniumPadding(); LongestAxisSetter(); }

	AABB::AxisAliagnedBoundingBoxe(const Interval& x, const Interval& y, const Interval& z)
		: m_X(x), m_Y(y), m_Z(z) { s_NumberofBoundingBoxes++; MiniumPadding(); LongestAxisSetter(); }

	AABB::AxisAliagnedBoundingBoxe(const Point& a, const Point& b)
	{
		s_NumberofBoundingBoxes++;
		m_X = (a.x <= b.x) ? Interval(a.x, b.x) : Interval(b.x, a.x);
		m_Y = (a.y <= b.y) ? Interval(a.y, b.y) : Interval(b.y, a.y);
		m_Z = (a.z <= b.z) ? Interval(a.z, b.z) : Interval(b.z, a.z);
		MiniumPadding();
		LongestAxisSetter();
	}

	AABB::AxisAliagnedBoundingBoxe(const AABB& box0, const AABB& box1)
		: m_X(box0.m_X, box1.m_X), m_Y(box0.m_Y, box1.m_Y), m_Z(box0.m_Z, box1.m_Z) { s_NumberofBoundingBoxes++; MiniumPadding(); LongestAxisSetter(); }

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
#if SIMD | AVX512 == 1 && !defined(RTW_SSE2)
		const __m128d m128_InvertValue = { 1.0, -1.0 };
#endif
#if defined(RTW_AVX512) & (AVX512 == 1)
		const __mmask8 loadMask = 0b00111111;
		// load m_X, m_Y and m_Z into an AVX512 register
		__m512d m512_AxisBounds = _mm512_maskz_load_pd(loadMask, &m_X);

		// Load ray origin into an AVX512 Register and double each axis into 128 bit lanes
		const Vec3& rayOrigin = ray.origin();
		__m512d m512_RayOrigin = _mm512_set_pd(0.0, 0.0, rayOrigin.z, rayOrigin.z, rayOrigin.y, rayOrigin.y, rayOrigin.x, rayOrigin.x);


		// Load ray inverted direction into an AVX512 Register and double each axis into 128 bit lanes
		const Vec3& rayInvDirection = ray.invDirection();
		__m512d m512_RayInvDirection = _mm512_set_pd(0.0, 0.0, rayInvDirection.z, rayInvDirection.z, rayInvDirection.y, rayInvDirection.y, rayInvDirection.x, rayInvDirection.x);

		// create T in AVX512
		__m512d m512_T = _mm512_mul_pd(_mm512_sub_pd(m512_AxisBounds, m512_RayOrigin), m512_RayInvDirection);

#if __clang__
		// split interleaving T into 2 AVX256 registers for comparison of smaller and larger
		__m256d m256_TMin = { m512_T[0], m512_T[2], m512_T[4], m512_T[6] };
		__m256d m512_TMax = { m512_T[1], m512_T[3], m512_T[5], m512_T[7] };

		// preform comparison
		__mmask8 TSwapMask = _mm256_cmp_pd_mask(m256_TMin, m512_TMax, _CMP_GT_OS);
		// scale the bit mask from AVX2 to avx512so that it load correctly 
		TSwapMask = ScaleBits(TSwapMask, 2);

		// make swaps as necessary
		m512_T = _mm512_mask_permute_pd(m512_T, TSwapMask, m512_T, 0b01010101);
		
		// creates a swap mask to swap in 128 bit flips
//		const __m512i m512_SwapBitMap = _mm512_set_epi64(6, 7, 4, 5, 2, 3, 0, 1);
//		const __mmask8 m512_SwapBitMask = 0b10101010;
//
// 		// creates the swapped T register
//		__m512d m512_T128BitSwaped = _mm512_permutexvar_pd(m512_SwapBitMap, m512_T);
// 
//		// creates a register of the minimum values of t such that the resistor will be (Xmin, Xmin, Ymin, Ymin, Zmin, Zmin, Amin, Amin)
//		__m512d m512_TMin = _mm512_min_pd(m512_T, m512_T128BitSwaped);
//		// creates a register of the maximum values of t such that the resistor will be (Xmax, Xmax, Ymax, Ymax, Zmax, Zmax, Amax, Amax)
//		__m512d m512_TMax = _mm512_max_pd(m512_T, m512_T128BitSwaped);
//
// 		// blends Tmin and Tmax so that m512_T is (Xmin, Xmax, Ymin, Ymax, Zmin, Zmax, Amin, Amax)
//		m512_T = _mm512_mask_blend_pd(m512_SwapBitMask, m512_TMin, m512_TMax);

#elif _MSC_VER
		// split interleaving T into 2 AVX256 registers for comparison of smaller and larger
		__m256d m256_TMin = { m512_T.m512d_f64[0], m512_T.m512d_f64[2], m512_T.m512d_f64[4], m512_T.m512d_f64[6] };
		__m256d m512_TMax = { m512_T.m512d_f64[1], m512_T.m512d_f64[3], m512_T.m512d_f64[5], m512_T.m512d_f64[7] };

		// preform comparison
		__mmask8 TSwapMask = _mm256_cmp_pd_mask(m256_TMin, m512_TMax, _CMP_GT_OS);
		// scale the bit mask from AVX2 to avx512so that it load correctly 
		TSwapMask = ScaleBits(TSwapMask, 2);

		// make swaps as necessary
		m512_T = _mm512_mask_permute_pd(m512_T, TSwapMask, m512_T, 0b01010101);
#else
#error only supports clang and msc
#endif
		// load rayT and the inverted mask and broadcast it duplicating it in each 128 bit lane except the last one but that is not needed
		__m512d m512_RayT = _mm512_maskz_broadcast_f64x2(loadMask, rayT.GetAsVector().data);
		const __m512d m512_InvertValue = _mm512_maskz_broadcast_f64x2(loadMask, m128_InvertValue);

		// negates T and rayT so that the same comparison can be computed on each x and y (as per 128 bit registers)
		__m512d m512_AltInvT = _mm512_mul_pd(m512_T, m512_InvertValue);
		__m512d m512_AltInvRayT = _mm512_mul_pd(m512_RayT, m512_InvertValue);

		// performs comparison and removes the negation
		__m512d m512_MaxMinT = _mm512_mul_pd(_mm512_max_pd(m512_AltInvRayT, m512_AltInvT), m512_InvertValue);

		// checks each 128 bit pair to see if the AABB has been hit 
		return CheckOrderedPairs0To5(m512_MaxMinT);

#else // RTW_AVX512
		for (Axis axis = Axis::x; axis <= Axis::z; axis++)
		{
			const Interval& axisBounds = GetAxisInterval(axis);
			const double rAxisOrigin = ray.origin()[+axis];
			const double rAxisInvDirection = ray.invDirection()[+axis];

			glm::dvec2 t = (axisBounds.GetAsVector() - rAxisOrigin) * rAxisInvDirection;

			if (t.x > t.y)
				std::swap(t.x, t.y);

#if (defined(RTW_AVX2) | defined(RTW_AVX512)) & (SIMD == 1)
			// load t and rayT into SIMD register while negating y so that the same comparison can be computed on x and y
			__m128d m128_T = _mm_mul_pd(m128_InvertValue, t.data);
			__m128d m128_RayT = _mm_mul_pd(m128_InvertValue, rayT.GetAsVector().data);

#ifdef RTW_AVX512
			// perform comparison
			__mmask8 bitMask = _mm_cmp_pd_mask(m128_T, m128_RayT, _CMP_GT_OS);
			// load based on mask rayT or T into register
			// this also resets the negation on on y as we pull the data from the original scaler data in memory but the compiler
			// will optimize that out and use the memory load from earlier.
			m128_RayT = _mm_mask_load_pd(rayT.GetAsVector().data, bitMask, &t.data);

#else // RTW_AVX512
			// perform comparison
			__m128d bitMask = _mm_cmpgt_pd(m128_T, m128_RayT);
			// load based on mask rayT or T into register
			m128_RayT = _mm_blendv_pd(rayT.GetAsVector().data, t.data, bitMask);
#endif // RTW_AVX512
#if __clang__
			// check if x is smaller than or equal y if so return as the ray has missed the box
			if (m128_RayT[0] >= m128_RayT[1])
				return false;
#elif _MSC_VER
			// check if x is smaller than or equal y if so return as the ray has missed the box
			if (m128_RayT.m128d_f64[0] >= m128_RayT.m128d_f64[1])
				return false;
#else
#error only supports clang and msc
#endif
			// store rayT back into scaler rayT
			const_cast<glm::dvec2&>(rayT.GetAsVector()).data = m128_RayT;

#else // defined(RTW_AVX2) | defined(RTW_AVX512) & (SIMD == 1)
			if (t.x > rayT.GetMin()) 
				rayT.SetMin(t.x);
			if (t.y < rayT.GetMax())
				rayT.SetMax(t.y);

			if (rayT.GetMin() >= rayT.GetMax())
				return false;
#endif // defined(RTW_AVX2) | defined(RTW_AVX512) & (SIMD == 1)
		}
		return true;
#endif // RTW_AVX512
	}

	RTW::AxisAliagnedBoundingBoxe& AxisAliagnedBoundingBoxe::operator=(const AxisAliagnedBoundingBoxe& aabb)
	{
		this->m_X = aabb.m_X;
		this->m_Y = aabb.m_Y;
		this->m_Z = aabb.m_Z;
		this->m_LongestAxis = aabb.m_LongestAxis;
		return *this;
	}

	void AxisAliagnedBoundingBoxe::Expand(const AxisAliagnedBoundingBoxe& newAABB)
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

	void AxisAliagnedBoundingBoxe::LongestAxisSetter()
	{
		m_LongestAxis = (m_X.Size() > m_Y.Size()) ?
					   ((m_X.Size() > m_Z.Size()) ? AABB::Axis::x : AABB::Axis::z) :
					   ((m_Y.Size() > m_Z.Size()) ? AABB::Axis::y : AABB::Axis::z);
	}

	void AxisAliagnedBoundingBoxe::MiniumPadding()
	{
		static constexpr double minium = 1e-4;
		if (m_X.Size() < minium)
			m_X = m_X.Expand(minium);
		if (m_Y.Size() < minium)
			m_Y = m_Y.Expand(minium);
		if (m_Z.Size() < minium)
			m_Z = m_Z.Expand(minium);
	}
}
