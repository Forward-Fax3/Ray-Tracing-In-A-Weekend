#include <ostream>

#include "glm/glm.hpp"

#include "Core.h"
#include "Interval.h"
#include "WriteColour.h"


namespace RTW
{
	void WriteColour(std::ostream& out, const Colour& colour)
	{
		const Vec3 gamma(2.4);
		Colour gammaCorrectedColour = glm::pow(colour, 1.0 / gamma);

		glm::i16vec3 colourTemp = Interval(0.0, 0.999).Clamp(gammaCorrectedColour) * 256.0;

		out << colourTemp.r << ' ' << colourTemp.g << ' ' << colourTemp.b << '\n';
	}
}
