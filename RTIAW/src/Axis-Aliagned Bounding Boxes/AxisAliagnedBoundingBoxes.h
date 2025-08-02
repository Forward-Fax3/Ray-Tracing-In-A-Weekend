#pragma once
#include "Core.h"
#include "Interval.h"
#include "Ray.h"

#include <atomic>


namespace RTW
{
	class AxisAliagnedBoundingBoxe
	{
	public:
		enum class Axis : uint8_t { x = 0, y, z, none };

	public:
		inline AxisAliagnedBoundingBoxe() { s_NumberofBoundingBoxes++; }
		AxisAliagnedBoundingBoxe(const Interval& interval);
		AxisAliagnedBoundingBoxe(const Interval& x, const Interval& y, const Interval& z);
		AxisAliagnedBoundingBoxe(const Point& a, const Point& b);
		AxisAliagnedBoundingBoxe(const AxisAliagnedBoundingBoxe& box0, const AxisAliagnedBoundingBoxe& box1);

		inline ~AxisAliagnedBoundingBoxe() { s_NumberofBoundingBoxes--; }

		AxisAliagnedBoundingBoxe& operator=(const AxisAliagnedBoundingBoxe& aabb)
		{
			this->m_X = aabb.m_X;
			this->m_Y = aabb.m_Y;
			this->m_Z = aabb.m_Z;
			this->m_LongestAxis = aabb.m_LongestAxis;
			return *this;
		}

		const Interval& GetAxisInterval(const Axis& axis) const;

		bool IsHit(const Ray& ray, Interval rayT) const;

		void Expand(const AxisAliagnedBoundingBoxe& newAABB);

		double GetSurfaceArea() const { return 2.0 * (m_X.Size() * m_Y.Size() + m_X.Size() * m_Z.Size() + m_Y.Size() * m_Z.Size()); }

		AxisAliagnedBoundingBoxe::Axis LongestAxis() const { return m_LongestAxis; }

		static inline size_t GetNumberofBBs() { return s_NumberofBoundingBoxes; }
		static inline void ResetNumberOfBBs() { s_NumberofBoundingBoxes = 0; }

		static const AxisAliagnedBoundingBoxe empty, univers;

	private:
		void LongestAxisSetter();

	private:
		Interval m_X, m_Y, m_Z;
		AxisAliagnedBoundingBoxe::Axis m_LongestAxis;

		static std::atomic<size_t> s_NumberofBoundingBoxes;
	};

	using AABB = AxisAliagnedBoundingBoxe;

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
