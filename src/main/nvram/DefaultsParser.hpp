#pragma once

#include <vector>

#include <file/File.hpp>

namespace mpc::ui {
	class UserDefaults;
}

namespace mpc::file::all {
	class Defaults;
}

namespace mpc::nvram {

	class DefaultsParser
	{

	public:
		static mpc::file::all::Defaults AllDefaultsFromFile(moduru::file::File& file);

	public:
		std::vector<char> saveBytes{};

	public:
		std::vector<char> getBytes();

		DefaultsParser(mpc::ui::UserDefaults& ud);
	};
}
