#pragma once
#include "Core.h"
#include "BaseNoise.h"


namespace RTW
{
	class PerlinNoise : public BaseNoise
	{
	public:
		PerlinNoise();

		virtual double Noise(const Point& point) override;

	private:
		static double TrilinearInterpilation(const Vec3 samples[2][2][2], const Vec3& cords);
		static void PerlinNoiseGeneratePermute(glm::vec<3, size_t, glm::defaultp>* p);
		static void Permute(glm::vec<3, size_t, glm::defaultp>* p, size_t n);

	private:
		static constexpr size_t s_NumberOfPoints = 256;
		Vec3 m_RandVec3s[s_NumberOfPoints];
		glm::vec<3, size_t, glm::defaultp> m_Permutes[s_NumberOfPoints];
	};
}
