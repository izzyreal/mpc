#pragma once

#include <file/File.hpp>

#include <vector>

namespace mpc::file::all
{
	class Defaults;
}

namespace mpc::nvram
{

	class DefaultsParser
	{

	public:
		static mpc::file::all::Defaults AllDefaultsFromFile(moduru::file::File& file);
		std::vector<char> saveBytes{};
		std::vector<char> getBytes();

		DefaultsParser();
	};
}
