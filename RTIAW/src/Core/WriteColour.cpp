#include <ostream>

#include "glm/glm.hpp"

#include "Core.h"
#include "WriteColour.h"


namespace RTW
{
	void WriteColour(std::ostream& out, const Colour& colour)
	{
		glm::i16vec3 colourTemp = colour;
//		out << colourTemp.r << ' ' << colourTemp.g << ' ' << colourTemp.b << '\n';
		std::println(out, "{} {} {}", colourTemp.r, colourTemp.g, colourTemp.b);
	}
}
