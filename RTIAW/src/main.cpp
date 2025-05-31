#include <iostream>

#include "glm/glm.hpp"

#include "Core.h"
#include "WriteColour.h"


int main()
{
	// Image
	int image_width = 256;
	int image_height = 256;

	// Render
	std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

	for (int j = 0; j < image_height; ++j)
	{
		std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;

		for (int i = 0; i < image_width; i++)
		{
			RTW::Colour colour = RTW::Colour(double(i) / (image_width - 1), double(j) / (image_height - 1), 0.0);
			RTW::WriteColour(std::cout, colour);
		}
	}

	std::clog << "\rDone.                 \n";
}