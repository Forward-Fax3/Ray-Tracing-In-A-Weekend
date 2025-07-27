#pragma once
#include <cstdint>
#include <limits>

#include "glm/glm.hpp"

#pragma warning (disable: 4201)


namespace RTW
{
	using Vec3 = glm::dvec3;
//	using Vec3 = glm::vec3;
	using Colour = Vec3;
	using Point = Vec3;
	using UV = glm::dvec2;
//	using UV = glm::vec2;
	static constexpr double doubleInf = std::numeric_limits<double>::infinity();

	Vec3 RandomOnHemisphere(const Vec3& normal);

	Vec3 RandomUnitVector();

	Vec3 RandomOnUnitDisk();

//	template <typename Tout, uint64_t t_ScaleIn, typename Tin>
//	std::enable_if_t<std::numeric_limits<Tin>::is_signed && std::numeric_limits<Tout>::is_signed, Tout> ScaleBits<Tout, t_ScaleIn>(Tin in)
//	{
//		unsigned mask = 0x0FF;
//		Tout out = static_cast<Tout>(in);
//		constexpr uint8_t N = sizeof(Tout) * 2;
//		for (uint64_t scale = t_ScaleIn; scale != 0; scale /= 2)
//		{
//			Tout shift = scale * (N - 1);
//			mask &= ~(mask << scale);
//			mask |= mask << (scale * N);
//			out = (out | out << shift) & mask;
//		}
//		out *= (1 << N) - 1;
//		return out;
//	}

	inline __mmask8 ScaleBits(__mmask8 in, size_t scale)
	{
		unsigned mask = 0x0FF;
		__mmask8 out = in;
		constexpr uint8_t N = sizeof(__mmask8) * 2;
		for (; scale != 0; scale /= 2)
		{
			__mmask8 shift = scale * (N - 1);
			mask &= ~(mask << scale);
			mask |= mask << (scale * N);
			out = (out | out << shift) & mask;
		}
		out *= (1 << N) - 1;
		return out;
	}

//	template <typename T, uint64_t t_Scale>
//	T ScaleBits<t_Scale>(T in)
//	{
//		return ScaleBits<T, t_Scale>(in;)
//	}
}
