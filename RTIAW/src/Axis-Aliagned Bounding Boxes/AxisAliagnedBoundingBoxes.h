#pragma once
#include "Core.h"
#include "Interval.h"
#include "Ray.h"

#include <atomic>
#include <utility>

#define SIMD 1


namespace RTW
{
	class alignas(64) AxisAliagnedBoundingBoxes
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

		RTW_FORCE_INLINE bool IsHit(const Ray& ray, Interval rayT) const;

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
#if _HAS_CXX23
		axis = static_cast<AABB::Axis>(std::to_underlying(axis) + 1);
#else
		axis = static_cast<AABB::Axis>(static_cast<uint8_t>(axis) + 1);
#endif
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
#if _HAS_CXX23
		return std::to_underlying(axis);
#else
		return static_cast<std::underlying_type_t<AABB::Axis>>(axis);
#endif
	}

	RTW_FORCE_INLINE bool AABB::IsHit(const Ray& ray, Interval rayT) const
	{
#if RTW_AVX512 & SIMD
		const __m512i m512_SwapBitMap = _mm512_setr_epi64(1, 0, 3, 2, 5, 4, 7, 6);

		const __m512d m512_AltNegMul = _mm512_setr_pd(0.0, -0.0, 0.0, -0.0, 0.0, -0.0, 0.0, -0.0);

		__m512i m512_loadyloads = _mm512_setr_epi64(0, 0, 1, 1, 2, 2, 3, 3);


		// load m_X, m_Y and m_Z into an AVX512 register
		__m512d m512_AxisBounds = _mm512_load_pd(this);

		// Load ray origin into an AVX512 Register and double each axis into 128 bit lanes
		__m512d m512_RayOrigin = _mm512_permutexvar_pd(m512_loadyloads, _mm512_castpd256_pd512(ray.origin().data));

		// Load ray inverted direction into an AVX512 Register and double each axis into 128 bit lanes
		__m512d m512_RayInvDirection = _mm512_permutexvar_pd(m512_loadyloads, _mm512_castpd256_pd512(ray.invDirection().data));


		// create T in AVX512
		__m512d m512_T = _mm512_mul_pd(_mm512_sub_pd(m512_AxisBounds, m512_RayOrigin), m512_RayInvDirection);

		// creates the swapped T register
		__m512d m512_T128BitSwaped = _mm512_permutexvar_pd(m512_SwapBitMap, m512_T);


		// performs an xor so that min and max operations can be performed
		__m512d m512_AltNegT = _mm512_xor_pd(m512_T, m512_AltNegMul);
		__m512d m512_AltNegSwapedT = _mm512_xor_pd(m512_T128BitSwaped, m512_AltNegMul);

		// minimums m512_AltNegT and m512_AltNegSwapedT so that m512_T is (Xmin, -Xmax, Ymin, -Ymax, Zmin, -Zmax, Amin, -Amax)
		__m512d m512_MinNegMaxT = _mm512_min_pd(m512_AltNegT, m512_AltNegSwapedT);


		// creates a m128_AltNegTest register then shrinks it to the smallest size
		__m128d m128_AltNegTest = _mm_max_pd(_mm512_extractf64x2_pd(m512_MinNegMaxT, 0), _mm512_extractf64x2_pd(m512_MinNegMaxT, 1));
		m128_AltNegTest = _mm_max_pd(m128_AltNegTest, _mm512_extractf64x2_pd(m512_MinNegMaxT, 2));

		// get rayT and perform max with that as well to make sure that MinNegMaxT is inside the rays boundary
		__m128d m128_AltNegRayT = _mm_xor_pd(rayT.GetAsVector().data, _mm512_extractf64x2_pd(m512_AltNegMul, 0));
		m128_AltNegTest = _mm_max_pd(m128_AltNegTest, m128_AltNegRayT);

		// inverts the values back for final comparison
		__m128d m128_Test = _mm_xor_pd(m128_AltNegTest, _mm512_extractf64x2_pd(m512_AltNegMul, 0));

		// test the boundary of the m128_Test minimum and maximum to make sure max is not smaller than or equal to the minimum bound
		// using shufpd and comisd instructions as other wise the compiler will do the same thing in memory instead
		// of just using built in instruction
		__m128d m128_MaxValue = _mm_shuffle_pd(m128_Test, m128_Test, 1);
		return _mm_comilt_sd(m128_Test, m128_MaxValue);

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
			const __m128d m128_InvertValue = _mm_set_pd(-0.0, 0.0);

			// load t and rayT into SIMD register while negating y so that the same comparison can be computed on x and y
			__m128d m128_T = _mm_xor_pd(m128_InvertValue, t.data);
			__m128d m128_RayT = _mm_xor_pd(m128_InvertValue, rayT.GetAsVector().data);

			// perform comparison
			__m128d m128_BitMask = _mm_cmpgt_pd(m128_T, m128_RayT);

			// load based on mask rayT or T into register
#if RTW_AVX2
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
}

#undef SIMD
