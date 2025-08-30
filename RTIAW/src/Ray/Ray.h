#pragma once
#include "glm/glm.hpp"

#include "Core.h"


namespace RTW
{
	class Ray
	{
	public:
		Ray();
		Ray(const Point& origin, const Vec3& direction);
		Ray(const Point& origin, const Vec3& direction, double time);

		inline const Point& origin() const { return m_Origin; }
		inline const Vec3& direction() const { return m_Direction; }
		inline const Vec3& invDirection() const { return m_InvDirection; }

		Point at(double distance) const;

		double time() const { return m_Time; }

	private:
#ifndef _DEBUG
		union
		{
			Point m_Origin;
			glm::dvec4 m_OriginTime;

			struct
			{
				glm::highp_dvec3 padding;
				double m_Time;
			};
		};
		Vec3 m_Direction;
		Vec3 m_InvDirection;
#else
		Point m_Origin;
		Vec3 m_Direction;
		Vec3 m_InvDirection;
		double m_Time;
#endif
	};
}
