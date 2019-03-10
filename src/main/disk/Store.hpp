#pragma once

#include <string>

namespace mpc {
	namespace disk {

		class Stores;

		class Store final
		{

		public:
			bool raw { false };
			std::string path { "" };
			int totalSpace { 0 };
			std::string volumeLabel { "" };

		public:
			std::string toString();
			Store(Stores* stores, std::string path, int totalSpace, std::string volumeLabel, bool raw);

		public:
			Stores* stores{ nullptr };

		};

	}
}
