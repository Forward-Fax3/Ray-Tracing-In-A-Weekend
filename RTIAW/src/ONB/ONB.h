#pragma once
#include "Core.h"
#include <array>


namespace RTW
{
	class ONB
	{
	public:
		ONB() = delete;
		explicit ONB(const Vec3& n);

		RTW_FORCE_INLINE const Vec3& operator[](const size_t index) const { return m_Axis[index]; }

		RTW_FORCE_INLINE const Vec3& U() const { return m_Axis[0]; }
		RTW_FORCE_INLINE const Vec3& V() const { return m_Axis[1]; }
		RTW_FORCE_INLINE const Vec3& W() const { return m_Axis[2]; }

		RTW_FORCE_INLINE Vec3 Transform(const Vec3& a) const
		{
			return a.x * U() + a.y * V() + a.z * W();
		}

	private:
		std::array<Vec3, 3> m_Axis;
	};
}
