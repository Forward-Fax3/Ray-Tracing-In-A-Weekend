#include <format>
#include <fstream>
#include <filesystem>
#include <chrono>
#include <iostream>

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
	WriteFile::WriteFile()
	{
		FullFilepath fullFilePath = GetFilePath();
		if (!CreateDir(fullFilePath.filepath))
			return;

		m_ofs.open(fullFilePath.filepath + '\\' + fullFilePath.filename);
	}

	void WriteFile::WriteToFile(const std::vector<Colour>& pixelData, int16_t width, int16_t height)
	{
		m_ofs << "P3 " << width << ' ' << height << ' ' << std::numeric_limits<uint16_t>::max() << ' ';

		for (const glm::u16vec3 colour : pixelData)
			m_ofs << std::format("{} ", colour);

		m_ofs.close();
	}

	WriteFile::FullFilepath WriteFile::GetFilePath()
	{
		auto const time = std::chrono::current_zone()->to_local(std::chrono::system_clock::now());
		return { std::format(R"(.\Images\{:%Y\%B\%d})", time), std::format("{:%H-%M}-{:%S}.ppm", time, std::chrono::floor<std::chrono::seconds>(time)) };
	}

	bool WriteFile::CreateDir(std::string_view filepath)
	{
		try
		{
			std::filesystem::create_directories(filepath);
			return true;
		}
		catch (std::filesystem::filesystem_error& e)
		{
			std::cout << e.what() << '\n';
			return false;
		}
	}
}
