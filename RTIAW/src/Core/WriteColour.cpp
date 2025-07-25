#include <ostream>

#include "glm/glm.hpp"

#include "Core.h"
#include "WriteColour.h"


namespace RTW
{
	void WriteColour(std::ostream& out, const Colour& colour)
	{
		glm::i16vec3 colourTemp = colour;
#if (_cplusplus == 202302L)
		std::println(out, "{} {} {}", colourTemp.r, colourTemp.g, colourTemp.b);
#else
		out << colourTemp.r << ' ' << colourTemp.g << ' ' << colourTemp.b << '\n';
#endif
	}
}
