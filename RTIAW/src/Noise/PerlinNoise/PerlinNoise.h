#pragma once
#include "Core.h"
#include "BaseNoise.h"

#include <array>


namespace RTW
{
	class PerlinNoise : public BaseNoise
	{
	private:
		static constexpr size_t s_NumberOfPoints = 256;

	public:
		PerlinNoise();
		~PerlinNoise() override = default;

		double Noise(const Point& point) const override;
		double Turbulation(const Point& point, size_t depth) const override;

	private:
		static double PerlinInterpilation(const std::array<std::array<std::array<Vec3, 2>, 2>, 2>& samples, const Vec3& cords);
		static void PerlinNoiseGeneratePermute(std::array<glm::vec<3, size_t, glm::defaultp>, s_NumberOfPoints>& p);
		static void Permute(std::array<glm::vec<3, size_t, glm::defaultp>, s_NumberOfPoints>& p);

	private:
		std::array<Vec3, s_NumberOfPoints> m_RandVec3s;
		std::array<glm::vec<3, size_t, glm::defaultp>, s_NumberOfPoints> m_Permutes;
	};
}
