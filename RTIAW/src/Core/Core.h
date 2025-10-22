#pragma once
#include <cstdint>
#include <limits>

#include "ctpl_stl.h"

// globally disable nameless struct warning
#pragma warning (disable: 4201)
// @SuppressWarnings("S6232")

#if defined(__INTEL_COMPILER) || defined(__INTEL_LLVM_COMPILER)
	#define RTW_INTEL_COMPILER 1
#endif

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#define RTW_FORCE_INLINE GLM_INLINE


namespace RTW
{
	RTW_FORCE_INLINE void unreachable() noexcept
	{
#if _HAS_CXX23
		std::unreachable();
#elif defined(_MSC_VER) && !defined(__clang__)
		__assume(false);
#else // GCC, Clang
		__builtin_unreachable();
#endif
	}

	using Vec2 = glm::dvec2;
	using Vec3 = glm::dvec3;
	using Colour = Vec3;
	using Point = Vec3;
	using UV = Vec2;
	static constexpr double doubleInf = std::numeric_limits<double>::infinity();

	using UVvec3 = glm::dmat2x3;
	using UVWvec3 = glm::dmat3;

	Vec3 RandomOnHemisphere(const Vec3& normal);

	Vec3 RandomUnitVector();

	Vec3 RandomOnUnitDisk();

	extern ::ctpl::thread_pool g_Threads;

	template <typename Tout, uint64_t t_ScaleIn, typename Tin>
	inline Tout ScaleBits(Tin in) requires std::numeric_limits<Tout>::is_integer && std::numeric_limits<Tin>::is_integer
	{
		auto mask = std::numeric_limits<Tout>::max();
		auto out = static_cast<Tout>(in);
		constexpr uint8_t N = sizeof(Tout) * 2;
		for (uint64_t scale = t_ScaleIn; scale != 0; scale /= 2)
		{
			auto shift = static_cast<Tout>(scale * (N - 1));
			mask &= ~(mask << scale);
			mask |= mask << (scale * N);
			out = (out | out << shift) & mask;
		}
		out *= (1 << N) - 1;
		return out;
	}

	template <uint64_t t_Scale, typename T>
	inline T ScaleBits(T in) requires std::numeric_limits<T>::is_integer
	{
		return ScaleBits<T, t_Scale>(in);
	}

	static RTW_FORCE_INLINE auto RotateAroundPoint(double rad, const Point& center, const Vec3& axis)
	{
		auto t1 = glm::translate(glm::dmat4(1.0), -center);
		auto r = glm::rotate(glm::dmat4(1.0), rad, axis);
		auto t2 = glm::translate(glm::dmat4(1.0), center);
		return t2 * r * t1;
	}

	static RTW_FORCE_INLINE auto RotateAroundPoint3D(const Vec3& radVec3, const Point& point, const Point& center)
	{
		auto temp = glm::translate(glm::dmat4(1.0), point);
		temp = RotateAroundPoint(radVec3.x, center, Vec3(0.0, 1.0, 0.0)) * temp;
		temp = RotateAroundPoint(radVec3.y, center, Vec3(0.0, 0.0, 1.0)) * temp;
		temp = RotateAroundPoint(radVec3.z, center, Vec3(1.0, 0.0, 0.0)) * temp;

		return temp[3];
	}
}
