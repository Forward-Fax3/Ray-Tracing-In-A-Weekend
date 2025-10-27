#include "ONB.h"


namespace RTW
{
	RTW::ONB::ONB(const Vec3& n)
	{
		m_Axis[2] = glm::normalize(n);

		Vec3 a = (glm::abs(m_Axis[2].x) > 0.9) ? Vec3(0.0, 1.0, 0.0) : Vec3(1.0, 0.0, 0.0);
		m_Axis[1] = glm::normalize(glm::cross(m_Axis[2], a));
		m_Axis[0] = glm::cross(m_Axis[2], m_Axis[1]);
	}
}
