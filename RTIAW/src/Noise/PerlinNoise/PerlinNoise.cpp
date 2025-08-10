#include "Core.h"
#include "BaseNoise.h"
#include "PerlinNoise.h"

#include "glm/glm.hpp"
#include "glm/gtc/random.hpp"

#include <array>

#define SIMD 0


namespace RTW
{
	PerlinNoise::PerlinNoise()
	{
		std::ranges::for_each(m_RandVec3s, [](Vec3& randVec) {
			randVec = glm::normalize(glm::linearRand(Vec3(-1.0), Vec3(1.0)));
		});

		PerlinNoiseGeneratePermute(m_Permutes);
	}

	double PerlinNoise::Noise(const Point& point) const
	{
		Vec3 uvw(point - glm::floor(point));

		auto indexes = static_cast<glm::vec<3, glm::int64, glm::defaultp>>(glm::floor(point));

		std::array<std::array<std::array<Vec3, 2>, 2>, 2> samples{{{}}};

		for (size_t i = 0; i < 2; i++)
			for (size_t j = 0; j < 2; j++)
			{
				glm::vec<4, glm::int64, glm::defaultp> ijk(i, j, 0, 1);
				glm::vec<4, glm::int64, glm::defaultp> tempIndexes(glm::xyzz(indexes));
				tempIndexes += ijk;

#if (defined(RTW_AVX2) || defined (RTW_AVX512)) && SIMD
				tempIndexes.data = _mm256_and_si256(tempIndexes.data, _mm256_set1_epi64x(s_NumberOfPoints - 1));
#elif defined(RTW_SSE2) && SIMD
				tempIndexes.data.setv(0, _mm_and_si128(tempIndexes.data.getv(0), glm::vec<2, size_t, glm::defaultp>(s_NumberOfPoints - 1).data));
				tempIndexes.data.setv(1, _mm_and_si128(tempIndexes.data.getv(1), glm::vec<2, size_t, glm::defaultp>(s_NumberOfPoints - 1).data));
#else
				tempIndexes = tempIndexes & (static_cast<glm::int64>(s_NumberOfPoints) - 1);
#endif

				size_t doublsIndex = m_Permutes[tempIndexes.x].x ^ m_Permutes[tempIndexes.y].y;

				samples[i][j][0] = m_RandVec3s[doublsIndex ^ m_Permutes[tempIndexes.z].z];
				samples[i][j][1] = m_RandVec3s[doublsIndex ^ m_Permutes[tempIndexes.w].z];
			}

		return PerlinInterpilation(samples, uvw);
	}

	double PerlinNoise::Turbulation(const Point& point, size_t depth) const
	{
		double accum = 0.0;
		double weight = 1.0;
		Point tempPoint(point);

		for (size_t i = 0; i < depth; i++)
		{
			accum += weight * Noise(tempPoint);
			weight *= 0.5;
			tempPoint *= 2.0;
		}

		return glm::abs(accum);
	}

	double PerlinNoise::PerlinInterpilation(const std::array<std::array<std::array<Vec3, 2>, 2>, 2>& samples, const Vec3& uvw)
	{
		Vec3 smoothedUVW = uvw * uvw * (3.0 - 2.0 * uvw);
		glm::dvec4 smoothedUVWW(glm::xyzz(smoothedUVW));

		double accum = 0.0;
		for (size_t i = 0; i < 2; i++)
			for (size_t j = 0; j < 2; j++)
			{
				glm::dvec4 ijk(static_cast<double>(i), static_cast<double>(j), 0.0, 1.0);
				glm::dvec4 weightV(glm::xyzz(uvw) - ijk);

#if defined(RTW_AVX2) || defined(RTW_AVX512) && !defined(__clang__) && SIMD // clang optimizes better with the simple version
				static const __m256d temp1_256bit = _mm256_set1_pd(1.0);
				__m256d tempIJK_256bit = _mm256_sub_pd(temp1_256bit, ijk.data);
				__m256d tempUVW_256bit = _mm256_sub_pd(temp1_256bit, smoothedUVWW.data);
				glm::dvec4 temp{};
				temp.data = _mm256_fmadd_pd(ijk.data, smoothedUVWW.data, _mm256_mul_pd(tempIJK_256bit, tempUVW_256bit));
#else
				glm::dvec4 temp(ijk * smoothedUVWW + (1.0 - ijk) * (1.0 - smoothedUVWW));
#endif

				double tempMultXY = temp.x * temp.y;
				std::array<Vec3, 2> weightVK{};
				weightVK[0].data = weightV.data;

#if defined(RTW_AVX2) || defined(RTW_AVX512) && SIMD
				weightVK[1].data = _mm256_permute_pd(weightV.data, 0b0110);
#elif defined(RTW_SSE2) && SIMD
				weightVK[1].data.setv(0, weightV.data.getv(0));
				glm_dvec2 weightVK01 = weightV.data.getv(1);
				weightVK[1].data.setv(1, _mm_shuffle_pd(weightVK01, weightVK01, 0b01));
#else
				weightVK[1] = Vec3(weightV.x, weightV.y, weightV.w);
#endif
				accum += tempMultXY * temp.z * glm::dot(samples[i][j][0], weightVK[0]) +
					tempMultXY * temp.w * glm::dot(samples[i][j][1], weightVK[1]);
			}
		return accum;
	}

	void PerlinNoise::PerlinNoiseGeneratePermute(std::array<glm::vec<3, size_t, glm::defaultp>, s_NumberOfPoints>& p)
	{
		for (size_t i = 0; i < s_NumberOfPoints; i++)
			p[i] = glm::vec<3, size_t, glm::defaultp>(i);

		Permute(p);
	}

	void PerlinNoise::Permute(std::array<glm::vec<3, size_t, glm::defaultp>, s_NumberOfPoints>& p)
	{
		for (size_t i = p.size() - 1; i > 0; i--)
		{
			glm::vec<3, size_t, glm::defaultp> target = linearRand(glm::vec<3, size_t, glm::defaultp>(0), glm::vec<3, size_t, glm::defaultp>(i));
			std::swap(p[i].x, p[target.x].x);
			std::swap(p[i].y, p[target.y].y);
			std::swap(p[i].z, p[target.z].z);
		}
	}
}
