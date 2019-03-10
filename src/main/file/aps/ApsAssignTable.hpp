#pragma once

#include <vector>

namespace mpc {
	namespace file {
		namespace aps {

			class ApsAssignTable
			{

			public: 
				std::vector<int> assignTable{};
				std::vector<char> saveBytes{};

			public: 
				std::vector<int> get();
				std::vector<char> getBytes();

			public: 
				ApsAssignTable(std::vector<char> loadBytes);
				ApsAssignTable(std::vector<int> assignTable);
			};

		}
	}
}
