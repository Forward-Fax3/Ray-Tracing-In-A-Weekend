#pragma once
#include "Core.h"
#include "Interval.h"
#include "Ray.h"

#include <atomic>

#define SIMD 1


namespace RTW
{
	class AxisAliagnedBoundingBoxes
	{
	public:
		enum class Axis : uint8_t { x = 0, y, z, none };

	public:
		inline explicit AxisAliagnedBoundingBoxes() { s_NumberOfBoundingBoxes++; }
		explicit AxisAliagnedBoundingBoxes(const Interval& interval);
		AxisAliagnedBoundingBoxes(const Interval& x, const Interval& y, const Interval& z);
		AxisAliagnedBoundingBoxes(const Point& a, const Point& b);
		AxisAliagnedBoundingBoxes(const AxisAliagnedBoundingBoxes& box0, const AxisAliagnedBoundingBoxes& box1);
		AxisAliagnedBoundingBoxes(const AxisAliagnedBoundingBoxes& other);

		inline ~AxisAliagnedBoundingBoxes() { s_NumberOfBoundingBoxes--; }

		AxisAliagnedBoundingBoxes& operator=(const AxisAliagnedBoundingBoxes& aabb) = default;

		bool IsBigger(const AxisAliagnedBoundingBoxes& otherAABB) const;

		const Interval& GetAxisInterval(const Axis& axis) const;

		__forceinline bool IsHit(const Ray& ray, Interval rayT) const;

		void Expand(const AxisAliagnedBoundingBoxes& newAABB);

		inline double GetSurfaceArea() const { return 2.0 * (m_X.Size() * m_Y.Size() + m_X.Size() * m_Z.Size() + m_Y.Size() * m_Z.Size()); }

		AxisAliagnedBoundingBoxes::Axis LongestAxis() const { return m_LongestAxis; }

		static inline size_t GetNumberofBBs() { return s_NumberOfBoundingBoxes; }
		static inline void ResetNumberOfBBs() { s_NumberOfBoundingBoxes = 0; }

		static const AxisAliagnedBoundingBoxes empty;
		static const AxisAliagnedBoundingBoxes univers;

	private:
		void LongestAxisSetter();
		void MiniumPadding();

	private:
		Interval m_X;
		Interval m_Y;
		Interval m_Z;
		Axis m_LongestAxis = Axis::none;

		static std::atomic<size_t> s_NumberOfBoundingBoxes;
	};

	using AABB = AxisAliagnedBoundingBoxes;

	inline AABB::Axis& operator++(AABB::Axis& axis)
	{
		axis = static_cast<AABB::Axis>(static_cast<uint8_t>(axis) + 1);
		return axis;
	}

	inline AABB::Axis operator++(AABB::Axis& axis, int)
	{
		AABB::Axis oldAxis = axis;
		++axis;
		return oldAxis;
	}

	static constexpr std::underlying_type_t<AABB::Axis> operator+(AABB::Axis axis) noexcept
	{
		return static_cast<std::underlying_type_t<AABB::Axis>>(axis);
	}

	bool AABB::IsHit(const Ray& ray, Interval rayT) const
	{
#if RTW_AVX512 & SIMD
		const __mmask8 loadMask = 0b00111111;

		const __m512i m512_SwapBitMap = _mm512_set_epi64(6, 7, 4, 5, 2, 3, 0, 1);
		const __mmask8 m512_SwapBitMask = 0b10101010;

		const __m512d m512_InvertValue = _mm512_set_pd(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
		const __m128d m128_InvertValue = _mm_set_pd(-1.0, 1.0);


		// load m_X, m_Y and m_Z into an AVX512 register
		__m512d m512_AxisBounds = _mm512_maskz_load_pd(loadMask, &m_X);

		// Load ray origin into an AVX512 Register and double each axis into 128 bit lanes
		const Vec3& rayOrigin = ray.origin();
		__m512d m512_RayOrigin = _mm512_setr_pd(rayOrigin.x, rayOrigin.x, rayOrigin.y, rayOrigin.y, rayOrigin.z, rayOrigin.z, 0.0, 0.0);

		// Load ray inverted direction into an AVX512 Register and double each axis into 128 bit lanes
		const Vec3& rayInvDirection = ray.invDirection();
		__m512d m512_RayInvDirection = _mm512_setr_pd(rayInvDirection.x, rayInvDirection.x, rayInvDirection.y, rayInvDirection.y, rayInvDirection.z, rayInvDirection.z, 0.0, 0.0);


		// create T in AVX512
		__m512d m512_T = _mm512_mul_pd(_mm512_sub_pd(m512_AxisBounds, m512_RayOrigin), m512_RayInvDirection);

		// creates the swapped T register
		__m512d m512_T128BitSwaped = _mm512_permutexvar_pd(m512_SwapBitMap, m512_T);

		// creates test1 register of the minimum values of t such that the resistor will be (Xmin, Xmin, Ymin, Ymin, Zmin, Zmin, Amin, Amin)
		__m512d m512_TMin = _mm512_min_pd(m512_T, m512_T128BitSwaped);
		// creates test1 register of the maximum values of t such that the resistor will be (Xmax, Xmax, Ymax, Ymax, Zmax, Zmax, Amax, Amax)
		__m512d m512_TMax = _mm512_max_pd(m512_T, m512_T128BitSwaped);

		// blends Tmin and Tmax so that m512_T is (Xmin, Xmax, Ymin, Ymax, Zmin, Zmax, Amin, Amax)
		m512_T = _mm512_mask_blend_pd(m512_SwapBitMask, m512_TMin, m512_TMax);


		// load rayT and the inverted mask and broadcast it duplicating it in each 128 bit lane except the last one but that is not needed
		__m512d m512_RayT = _mm512_maskz_broadcast_f64x2(loadMask, rayT.GetAsVector().data);

		// negates T and rayT so that the same comparison can be computed on each x and y (as per 128 bit registers)
		__m512d m512_AltInvT = _mm512_mul_pd(m512_T, m512_InvertValue);
		__m512d m512_AltInvRayT = _mm512_mul_pd(m512_RayT, m512_InvertValue);

		// performs AVX512 comparison
		__m512d m512_MaxMinT = _mm512_max_pd(m512_AltInvRayT, m512_AltInvT);
		// negation is not necessary yet the shrinking process would need to do the same negation

		// creates a test interval then shrinks it to the smallest size
		// the shrinking is performed here instead of with Intervals own shrink function as it adds unnecessary multiplications
		Interval test;
		test.SetMinMax(_mm_max_pd(_mm512_extractf64x2_pd(m512_MaxMinT, 0), _mm512_extractf64x2_pd(m512_MaxMinT, 1)));
		test.SetMinMax(_mm_mul_pd(_mm_max_pd(test.GetAsVector().data, _mm512_extractf64x2_pd(m512_MaxMinT, 2)), m128_InvertValue));

		// test the bound of the test interval to make sure max is not smaller than or equal to the minimum bound
		// using shufpd and comisd instructions as other wise the compiler will do the same thing in memory instead
		// of just using built in instruction
		__m128d maxValue = _mm_shuffle_pd(test.GetAsVector().data, test.GetAsVector().data, 1);
		return _mm_comilt_sd(test.GetAsVector().data, maxValue);

#else // !RTW_AVX512
		for (Axis axis = Axis::x; axis <= Axis::z; axis++)
		{
			const Interval& axisBounds = GetAxisInterval(axis);
			const double rAxisOrigin = ray.origin()[+axis];
			const double rAxisInvDirection = ray.invDirection()[+axis];

			glm::dvec2 t = (axisBounds.GetAsVector() - rAxisOrigin) * rAxisInvDirection;

			if (t.x > t.y)
				std::swap(t.x, t.y);

#if SIMD
			const __m128d m128_InvertValue = _mm_set_pd(-1.0, 1.0);

			// load t and rayT into SIMD register while negating y so that the same comparison can be computed on x and y
			__m128d m128_T = _mm_mul_pd(m128_InvertValue, t.data);
			__m128d m128_RayT = _mm_mul_pd(m128_InvertValue, rayT.GetAsVector().data);

			// perform comparison
			__m128d m128_BitMask = _mm_cmpgt_pd(m128_T, m128_RayT);

			// load based on mask rayT or T into register
#if RTW_AVX2 | RTW_AVX512
			rayT.SetMinMax(_mm_blendv_pd(rayT.GetAsVector().data, t.data, m128_BitMask));
#else // SSE2
			rayT.SetMinMax(_mm_or_pd(_mm_and_pd(m128_BitMask, t.data), _mm_andnot_pd(m128_BitMask, rayT.GetAsVector().data)));
#endif

#else // !SIMD
			if (t.x > rayT.GetMin())
				rayT.SetMin(t.x);
			if (t.y < rayT.GetMax())
				rayT.SetMax(t.y);
#endif // SIMD

			if (rayT.GetMin() >= rayT.GetMax())
				return false;
		}
		return true;
#endif // RTW_AVX512
	}
#undef SIMD
}
