#pragma once

#include <vector>

#include <file/File.hpp>

namespace mpc {

	namespace ui {
		class UserDefaults;
	}

	namespace file {
		namespace all {
			class Defaults;
		}
	}

	namespace nvram {

		class DefaultsParser
		{

		public:
			mpc::file::all::Defaults* defaults{};
			std::vector<char> saveBytes{};

		public:
			mpc::file::all::Defaults* getDefaults();

		public:
			std::vector<char> getBytes();

			DefaultsParser(moduru::file::File* file);
			DefaultsParser(mpc::ui::UserDefaults* ud);
		};

	}
}
