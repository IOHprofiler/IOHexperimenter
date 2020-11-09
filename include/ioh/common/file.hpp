#pragma once

#include <fstream>

#ifdef FSEXPERIMENTAL
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#else
#include <filesystem>
namespace fs = std::filesystem;
#endif

#include "log.hpp"

namespace ioh::common::file
{
	/**
	 * \brief Opens a file
	 * \param filename The path of the file to be opened
	 * \return a stream handle to the opened file
	 */
	inline std::ifstream open_file(fs::path& filename)
	{
		if (!fs::exists(filename))
			log::error("Cannot find file " + filename.generic_string());

		std::ifstream file(filename);
		if (!file.is_open())
			log::error("Cannot open file " + filename.generic_string());
		return file;
	}
}
