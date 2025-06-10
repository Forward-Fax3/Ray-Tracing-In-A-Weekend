#include "Core.h"
#include "Interval.h"
#include "Ray.h"
#include "AxisAliagnedBoundingBoxes.h"


namespace RTW
{
	AABB::AxisAliagnedBoundingBoxes(const Interval& interval)
		: m_X(interval), m_Y(interval), m_Z(interval) { s_NumberofBoundingBoxes++; }

	AABB::AxisAliagnedBoundingBoxes(const Interval& x, const Interval& y, const Interval& z)
		: m_X(x), m_Y(y), m_Z(z) { s_NumberofBoundingBoxes++; }

	AABB::AxisAliagnedBoundingBoxes(const Point& a, const Point& b)
	{
		s_NumberofBoundingBoxes++;
		m_X = (a.x <= b.x) ? Interval(a.x, b.x) : Interval(b.x, a.x);
		m_Y = (a.y <= b.y) ? Interval(a.y, b.y) : Interval(b.y, a.y);
		m_Z = (a.z <= b.z) ? Interval(a.z, b.z) : Interval(b.z, a.z);
	}

	AABB::AxisAliagnedBoundingBoxes(const AABB& box0, const AABB& box1)
		: m_X(box0.m_X, box1.m_X), m_Y(box0.m_Y, box1.m_Y), m_Z(box0.m_Z, box1.m_Z) { s_NumberofBoundingBoxes++; }

	const Interval& AABB::GetAxisInterval(const Axis& axis) const
	{
		switch (axis)
		{
		case Axis::x:
			return m_X;
		case Axis::y:
			return m_Y;
		case Axis::z:
			return m_Z;
		default:
			return Interval::Univers;
		}
	}

	bool AABB::IsHit(const Ray& ray, Interval rayT) const
	{
		for (Axis axis = Axis::x; axis <= Axis::z; axis++)
		{
			const Interval& axisBounds = GetAxisInterval(axis);
			const double rAxisDirection = 1.0 / ray.direction()[+axis];

			glm::dvec2 t = (glm::dvec2(axisBounds.GetMin(), axisBounds.GetMax()) - ray.origin()[+axis]) * rAxisDirection;

			if (t.x < t.y)
			{
				if (t.x > rayT.GetMin())
					rayT.SetMin(t.x);
				if (t.y < rayT.GetMax())
					rayT.SetMax(t.y);
			}
			else
			{
				if (t.y > rayT.GetMin())
					rayT.SetMin(t.y);
				if (t.x < rayT.GetMax())
					rayT.SetMax(t.x);
			}

			if (rayT.GetMax() <= rayT.GetMin())
				return false;
		}
		return true;
	}

	RTW::AABB::Axis AxisAliagnedBoundingBoxes::LongestAxis() const
	{
		return (m_X.Size() > m_Y.Size()) ?
			((m_X.Size() > m_Z.Size()) ? AABB::Axis::x : AABB::Axis::z) :
			((m_Y.Size() > m_Z.Size()) ? AABB::Axis::y : AABB::Axis::z);
	}

	size_t AxisAliagnedBoundingBoxes::s_NumberofBoundingBoxes = 0;

	const AABB AABB::empty = { Interval::Empty };
	const AABB AABB::univers = { Interval::Univers };
}
