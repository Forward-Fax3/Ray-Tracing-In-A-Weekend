#pragma once
#include "Core.h"
#include "Interval.h"
#include "Ray.h"

#include <atomic>


namespace RTW
{
	class AxisAliagnedBoundingBoxes
	{
	public:
		enum class Axis : uint8_t { x = 0, y, z, none };

	public:
		inline AxisAliagnedBoundingBoxes() { s_NumberofBoundingBoxes++; }
		explicit AxisAliagnedBoundingBoxes(const Interval& interval);
		AxisAliagnedBoundingBoxes(const Interval& x, const Interval& y, const Interval& z);
		AxisAliagnedBoundingBoxes(const Point& a, const Point& b);
		AxisAliagnedBoundingBoxes(const AxisAliagnedBoundingBoxes& box0, const AxisAliagnedBoundingBoxes& box1);

		inline ~AxisAliagnedBoundingBoxes() { s_NumberofBoundingBoxes--; }

		AxisAliagnedBoundingBoxes& operator=(const AxisAliagnedBoundingBoxes& aabb);

		const Interval& GetAxisInterval(const Axis& axis) const;

		bool IsHit(const Ray& ray, Interval rayT) const;

		void Expand(const AxisAliagnedBoundingBoxes& newAABB);

		double GetSurfaceArea() const { return 2.0 * (m_X.Size() * m_Y.Size() + m_X.Size() * m_Z.Size() + m_Y.Size() * m_Z.Size()); }

		AxisAliagnedBoundingBoxes::Axis LongestAxis() const { return m_LongestAxis; }

		static inline size_t GetNumberofBBs() { return s_NumberofBoundingBoxes; }
		static inline void ResetNumberOfBBs() { s_NumberofBoundingBoxes = 0; }

		static const AxisAliagnedBoundingBoxes empty;
		static const AxisAliagnedBoundingBoxes univers;

	private:
		void LongestAxisSetter();
		void MiniumPadding();

	private:
		Interval m_X;
		Interval m_Y;
		Interval m_Z;
		Axis m_LongestAxis = Axis::none;

		static std::atomic<size_t> s_NumberofBoundingBoxes;
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
