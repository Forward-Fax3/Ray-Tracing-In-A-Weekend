#include "Core.h"
#include "BaseTexture.h"
#include "SolidColour.h"
#include "CheckeredTexture.h"

#include <memory>

namespace RTW
{
	CheckeredTexture::CheckeredTexture(std::shared_ptr<BaseTexture> even, std::shared_ptr<BaseTexture> odd, double scale)
		: m_Even(even), m_Odd(odd), m_InvScale(1.0 / scale) {}

	CheckeredTexture::CheckeredTexture(const Colour& even, const Colour& odd, double scale)
		: m_Even(std::make_shared<SolidColour>(even)), m_Odd(std::make_shared<SolidColour>(odd)), m_InvScale(1.0 / scale) {}

	Colour CheckeredTexture::GetColour(const UV& uv, const Point& point) const
	{
		glm::i64vec3 intPoint = point * m_InvScale;

		bool isOdd = (intPoint.x + intPoint.y + intPoint.z) % 2;
		return isOdd ? m_Odd->GetColour(uv, point) : m_Even->GetColour(uv, point);
	}
}
