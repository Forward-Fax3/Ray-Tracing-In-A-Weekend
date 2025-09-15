#pragma once
#include <fstream>

#include "glm/glm.hpp"

#include "Core.h"


namespace RTW
{
	class WriteFile
	{
	public:
		WriteFile();
		~WriteFile() = default;

		inline bool IsOpen() const { return m_ofs.is_open(); }

		void WriteToFile(const std::vector<Colour>& pixelData, int16_t width, int16_t height);

	private:
		struct FullFilepath
		{
			std::string filepath;
			std::string filename;
		};

	private:
		static FullFilepath GetFilePath();
		static bool CreateDir(std::string_view filepath);

	private:
		std::ofstream m_ofs;
	};
}