#pragma once

#include <file/File.hpp>

#include <vector>

namespace mpc { class Mpc; }

namespace mpc::file::all
{
	class Defaults;
}

namespace mpc::nvram
{

	class DefaultsParser
	{

	public:
		static mpc::file::all::Defaults AllDefaultsFromFile(mpc::Mpc& mpc, moduru::file::File& file);
		std::vector<char> saveBytes{};
		std::vector<char> getBytes();

		DefaultsParser(mpc::Mpc& mpc);
	};
}
