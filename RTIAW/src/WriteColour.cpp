#include <ostream>

#include "glm/glm.hpp"

#include "Core.h"
#include "Interval.h"
#include "WriteColour.h"


namespace RTW
{
	void WriteColour(std::ostream& out, const Colour& colour)
	{
		glm::i16vec3 colourTemp = Interval(0.0, 0.999).Clamp(colour) * 256.0;

		out << colourTemp.r << ' ' << colourTemp.g << ' ' << colourTemp.b << '\n';
	}
}