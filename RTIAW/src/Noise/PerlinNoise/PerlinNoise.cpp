#include "Core.h"
#include "BaseNoise.h"
#include "PerlinNoise.h"

#include "glm/glm.hpp"
#include "glm/gtc/random.hpp"


namespace RTW
{
	PerlinNoise::PerlinNoise()
	{
		for (size_t i = 0; i < s_NumberOfPoints; i++)
			m_RandDoubles[i] = glm::linearRand(0.0, 1.0);

		PerlinNoiseGeneratePermute(m_Permutes);
	}

	double PerlinNoise::Noise(const Point& point)
	{
		Vec3 uvw = point - glm::floor(point);
		uvw = uvw * uvw * (3.0 - 2.0 * uvw);

		auto indexes = static_cast<glm::vec<3, size_t, glm::defaultp>>(glm::floor(point));

		double samples[2][2][2]{};

		for (size_t i = 0; i < 2; i++)
			for (size_t j = 0; j < 2; j++)
			{
				glm::vec<4, size_t, glm::defaultp> ijk(i, j, 0, 1);
				glm::vec<4, size_t, glm::defaultp> tempIndexes(indexes, indexes.z);
				tempIndexes += ijk;

#if (defined(RTW_AVX2) || defined (RTW_AVX512)) && (__x86_64 || __ppc64__ || _WIN64)
				__m256i tempIndexes_256bit = _mm256_and_epi64(tempIndexes.data, _mm256_set1_epi64x(s_NumberOfPoints - 1));
				_mm256_storeu_epi64(&tempIndexes, tempIndexes_256bit);
#elif defined(RTW_AVX2) || defined (RTW_AVX512)
				__m128i tempIndexes_256bit = _mm128_and_epi32(tempIndexes.data, _mm128_set1_epi32(s_NumberOfPoints - 1));
				_mm128_storeu_epi32(&tempIndexes, tempIndexes_256bit);
#else
				tempIndexes = tempIndexes & (s_NumberOfPoints - 1);
#endif

				size_t doublsIndex = m_Permutes[tempIndexes.x].x ^ m_Permutes[tempIndexes.y].y;

				samples[i][j][0] = m_RandDoubles[doublsIndex ^ m_Permutes[tempIndexes.z].z];
				samples[i][j][1] = m_RandDoubles[doublsIndex ^ m_Permutes[tempIndexes.w].z];
			}

		return TrilinearInterpilation(samples, uvw);
	}

	double PerlinNoise::TrilinearInterpilation(const double samples[2][2][2], const Vec3& uvw)
	{
		double accum = 0.0;
		for (size_t i = 0; i < 2; i++)
			for (size_t j = 0; j < 2; j++)
			{
				glm::dvec4 ijk(static_cast<double>(i), static_cast<double>(j), 0.0, 1.0);
				glm::dvec4 tempUVW(uvw, uvw.z);

#if defined(RTW_AVX2) || defined(RTW_AVX512)
				static const __m256d temp1_256bit = _mm256_set1_pd(1.0);
				__m256d tempIJK_256bit = _mm256_sub_pd(temp1_256bit, ijk.data);
				__m256d tempUVW_256bit = _mm256_sub_pd(temp1_256bit, tempUVW.data);
				glm::dvec4 temp{};
				_mm256_storeu_pd(&temp.x, _mm256_fmadd_pd(ijk.data, tempUVW.data, _mm256_mul_pd(tempIJK_256bit, tempUVW_256bit)));
#else
				glm::dvec4 temp(ijk * tempUVW + (1.0 - ijk) * (1.0 - tempUVW));
#endif

				double tempMultXY = temp.x * temp.y;
				accum += tempMultXY * temp.z * samples[i][j][0] + tempMultXY * temp.w * samples[i][j][1];
			}
		return accum;
	}

	void PerlinNoise::PerlinNoiseGeneratePermute(glm::vec<3, size_t, glm::defaultp>* p)
	{
		for (size_t i = 0; i < s_NumberOfPoints; i++)
			p[i] = glm::vec<3, size_t, glm::defaultp>(i);

		Permute(p, s_NumberOfPoints);
	}

	void PerlinNoise::Permute(glm::vec<3, size_t, glm::defaultp>* p, size_t n)
	{
		for (size_t i = n - 1; i > 0; i--)
		{
			glm::vec<3, size_t, glm::defaultp> target = linearRand(glm::vec<3, size_t, glm::defaultp>(0), glm::vec<3, size_t, glm::defaultp>(i));
			std::swap(p[i].x, p[target.x].x);
			std::swap(p[i].y, p[target.y].y);
			std::swap(p[i].z, p[target.z].z);
		}
	}
}
