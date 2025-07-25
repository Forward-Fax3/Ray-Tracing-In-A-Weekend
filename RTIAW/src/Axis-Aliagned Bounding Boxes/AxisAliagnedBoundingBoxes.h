#pragma once
#include "Core.h"
#include "Interval.h"
#include "Ray.h"


namespace RTW
{
	class AxisAliagnedBoundingBoxes
	{
	public:
		enum class Axis : uint8_t { x = 0, y, z };

	public:
		inline AxisAliagnedBoundingBoxes() { s_NumberofBoundingBoxes++; }
		AxisAliagnedBoundingBoxes(const Interval& interval);
		AxisAliagnedBoundingBoxes(const Interval& x, const Interval& y, const Interval& z);
		AxisAliagnedBoundingBoxes(const Point& a, const Point& b);
		AxisAliagnedBoundingBoxes(const AxisAliagnedBoundingBoxes& box0, const AxisAliagnedBoundingBoxes& box1);

		inline ~AxisAliagnedBoundingBoxes() { s_NumberofBoundingBoxes--; }

		const Interval& GetAxisInterval(const Axis& axis) const;

		bool IsHit(const Ray& ray, Interval rayT) const;

		AxisAliagnedBoundingBoxes::Axis LongestAxis() const { return m_LongestAxis; }

		static inline size_t GetNumberofBBs() { return s_NumberofBoundingBoxes; }

		static const AxisAliagnedBoundingBoxes empty, univers;

	private:
		AxisAliagnedBoundingBoxes::Axis LongestAxisSetter() const;

	private:
		Interval m_X, m_Y, m_Z;
		AxisAliagnedBoundingBoxes::Axis m_LongestAxis;

		static size_t s_NumberofBoundingBoxes;
	};

	using AABB = AxisAliagnedBoundingBoxes;

	inline AABB::Axis& operator++(AABB::Axis& axis)
	{
		axis = static_cast<AABB::Axis>(static_cast<uint8_t>(axis) + 1);
		return axis;
	}

	inline AABB::Axis operator++(AABB::Axis& axis, int)
	{
		AABB::Axis oldAxis = axis;
		++axis;
		return oldAxis;
	}

	static constexpr std::underlying_type_t<AABB::Axis> operator+(AABB::Axis axis) noexcept
	{
		return static_cast<std::underlying_type_t<AABB::Axis>>(axis);
	}
}
