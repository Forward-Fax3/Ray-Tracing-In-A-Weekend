#include "Core.h"
#include "RayHittable.h"
#include "BaseMaterial.h"
#include "Parallelogram.h"

#include <memory>
#include <array>


namespace RTW
{
	Parallelogram::Parallelogram(const Point& Q, const UVvec3& uv, std::shared_ptr<BaseMaterial> material)
		: m_Material(material), m_UV(uv), m_Q(Q)
	{
		Vec3 n = glm::cross(m_UV[0], m_UV[1]);
		m_Normal = glm::normalize(n);
		m_D = glm::dot(m_Normal, m_Q);
		m_W = n / glm::dot(n, n);

		CreateAABB();
	}

	bool Parallelogram::IsRayHit(const Ray& ray, const Interval& rayDistance, HitData& hitData) const
	{
		double denom = glm::dot(m_Normal, ray.direction());

		// if demon is 0 it is parallel to the parallelogram
		if (glm::abs(denom) < 1e-8)
			return false;

		double rayT = (m_D - glm::dot(m_Normal, ray.origin())) / denom;
		if (!rayDistance.Contains(rayT))
			return false;

		Point hitPoint = ray.at(rayT);
		UV tempUV = CalculateUV(hitPoint - m_Q);

		if (!Interval(0.0, 1.0).Contains(tempUV))
			return false;

		hitData.distance = rayT;
		hitData.point = hitPoint;
		hitData.uv = tempUV;
		hitData.material = m_Material;
		hitData.SetFaceNormal(ray, m_Normal);

		return true;
	}
	
	void Parallelogram::CreateAABB()
	{
		std::array<AABB, 2> AABBs{
			AABB(m_Q, m_Q + m_UV[0] + m_UV[1]),
			AABB(m_Q + m_UV[0], m_Q + m_UV[1])
		};
		m_AABB = AABB(AABBs[0], AABBs[1]);
	}

	UV Parallelogram::CalculateUV(const Point& p) const
	{
		return { glm::dot(m_W, glm::cross(p, m_UV[1])), glm::dot(m_W, glm::cross(m_UV[0], p))};
	}
}
