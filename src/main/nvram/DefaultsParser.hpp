#pragma once

#include <vector>
#include <memory>

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

		DefaultsParser(std::weak_ptr<mpc::ui::UserDefaults> ud);
	};
}
