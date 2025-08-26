#include <ostream>
#include <format>

#include "glm/glm.hpp"

#include "Core.h"
#include "WriteColour.h"


namespace std
{
	template<>
	struct formatter<glm::u16vec3>
	{
		constexpr auto parse(const format_parse_context& ctx) const
		{
			return ctx.begin();
		}

		auto format(const glm::u16vec3& colour, format_context& ctx) const
		{
			return format_to(ctx.out(), "{} {} {}", colour.x, colour.y, colour.z);
		}
	};
}

namespace RTW
{
	void WriteColour(std::ostream& out, const Colour& colour)
	{
		glm::u16vec3 colourTemp = colour;
#if _HAS_CXX23
		std::println(out, "{}", colourTemp);
#else
		out << std::format("{}\n", colourTemp);
#endif
	}
}
