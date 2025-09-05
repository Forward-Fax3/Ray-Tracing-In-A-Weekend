#include "Core.h"
#include "RayHittable.h"
#include "RayHittables.h"
#include "BaseMaterial.h"
#include "Parallelogram.h"

#include "glm/gtc/matrix_transform.hpp"

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
		if (std::abs(denom) < 1e-8)
			return false;

		double rayT = (m_D - glm::dot(m_Normal, ray.origin())) / denom;
		if (!rayDistance.Contains(rayT))
			return false;

		Point hitPoint = ray.at(rayT);
		UV tempUV = CalculateUV(hitPoint - m_Q);

		if (!Interval(0.0, 1.0).Contains(tempUV))
			return false;

		hitData.distance = rayT;
		hitData.SetPoint(hitPoint);
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

	std::shared_ptr<RTW::BaseRayHittable> CreateBox(const Point& pointA, const Point& pointB, std::shared_ptr<BaseMaterial> material)
	{
		// Construct the two opposite vertices with the minimum and maximum coordinates.
		Point min{};
		Point max{};

#if RTW_AVX2 || RTW_AVX512
		min.data = _mm256_min_pd(pointA.data, pointB.data);
		max.data = _mm256_max_pd(pointA.data, pointB.data);
#else
		min.data.setv(0, _mm_min_pd(pointA.data.getv(0), pointB.data.getv(0)));
		min.data.setv(1, _mm_min_pd(pointA.data.getv(1), pointB.data.getv(1)));

		max.data.setv(0, _mm_max_pd(pointA.data.getv(0), pointB.data.getv(0)));
		max.data.setv(1, _mm_max_pd(pointA.data.getv(1), pointB.data.getv(1)));
#endif

		Vec3 delta = max - min;
		Vec3 dX(delta.x, 0.0, 0.0);
		Vec3 dY(0.0, delta.y, 0.0);
		Vec3 dZ(0.0, 0.0, delta.z);

		auto box(std::make_shared<RayHittables>());
		box->reserve(6);
		box->add(std::make_shared<Parallelogram>(min, UVvec3(dX, dY), material)); // front
		box->add(std::make_shared<Parallelogram>(min, UVvec3(dZ, dY), material)); // right
		box->add(std::make_shared<Parallelogram>(min, UVvec3(dZ, dX), material)); // bottom
		box->add(std::make_shared<Parallelogram>(max, UVvec3(-dX, -dY), material)); // back
		box->add(std::make_shared<Parallelogram>(max, UVvec3(-dZ, -dY), material)); // left
		box->add(std::make_shared<Parallelogram>(max, UVvec3(-dZ, -dX), material)); // top

		return box;
	}

	static __forceinline auto RotateAroundPoint(double rad, const Point& center, const Vec3& axis)
	{
		auto t1 = glm::translate(glm::dmat4(1.0), -center);
		auto r = glm::rotate(glm::dmat4(1.0), rad, axis);
		auto t2 = glm::translate(glm::dmat4(1.0), center);
		return t2 * r * t1;
	}

	static __forceinline auto RotateAroundPoint3D(const Vec3& radVec3, const Point& point, const Point& center)
	{
		auto temp = glm::translate(glm::dmat4(1.0), point);
		temp = RotateAroundPoint(radVec3.x, center, Vec3(0.0, 1.0, 0.0)) * temp;
		temp = RotateAroundPoint(radVec3.y, center, Vec3(0.0, 0.0, 1.0)) * temp;
		temp = RotateAroundPoint(radVec3.z, center, Vec3(1.0, 0.0, 0.0)) * temp;

		return temp[3];
	}

	std::shared_ptr<RTW::BaseRayHittable> CreateBox(const Point& pointA, const Point& pointB, std::shared_ptr<BaseMaterial> material, const Vec3& degrees)
	{
		[[unlikely]]
		if (degrees == Vec3(0.0))
			return CreateBox(pointA, pointB, material);

		// Construct the two opposite vertices with the minimum and maximum coordinates.
		Point min{};
		Point max{};

#if RTW_AVX2 || RTW_AVX512
		min.data = _mm256_min_pd(pointA.data, pointB.data);
		max.data = _mm256_max_pd(pointA.data, pointB.data);
#else
		min.data.setv(0, _mm_min_pd(pointA.data.getv(0), pointB.data.getv(0)));
		min.data.setv(1, _mm_min_pd(pointA.data.getv(1), pointB.data.getv(1)));

		max.data.setv(0, _mm_max_pd(pointA.data.getv(0), pointB.data.getv(0)));
		max.data.setv(1, _mm_max_pd(pointA.data.getv(1), pointB.data.getv(1)));
#endif

		Vec3 delta = max - min;
		Vec3 radians = degrees * glm::radians(1.0); // using glm::radians(1.0) instead of glm::radians(degrees) as this way can be done with 1 multiply instruction instead of 3

		min = RotateAroundPoint3D(-radians, min, min + delta / 2.0);
		max = RotateAroundPoint3D(-radians, max, max - delta / 2.0);

		glm::dmat4 rot(1.0);
		rot = glm::rotate(rot, radians.x, Vec3(0.0, 1.0, 0.0));
		rot = glm::rotate(rot, radians.y, Vec3(0.0, 0.0, 1.0));
		rot = glm::rotate(rot, radians.z, Vec3(1.0, 0.0, 0.0));

		Vec3 dX(delta.x * Vec3(rot[0].x, rot[1].x, rot[2].x));
		Vec3 dY(delta.y * Vec3(rot[0].y, rot[1].y, rot[2].y));
		Vec3 dZ(delta.z * Vec3(rot[0].z, rot[1].z, rot[2].z));

		auto box(std::make_shared<RayHittables>());
		box->reserve(6);
		box->add(std::make_shared<Parallelogram>(min, UVvec3( dX,  dY), material)); // front
		box->add(std::make_shared<Parallelogram>(min, UVvec3( dZ,  dY), material)); // right
		box->add(std::make_shared<Parallelogram>(min, UVvec3( dZ,  dX), material)); // bottom
		box->add(std::make_shared<Parallelogram>(max, UVvec3(-dX, -dY), material)); // back
		box->add(std::make_shared<Parallelogram>(max, UVvec3(-dZ, -dY), material)); // left
		box->add(std::make_shared<Parallelogram>(max, UVvec3(-dZ, -dX), material)); // top

		return box;
	}
}
