#pragma once

#include <vector>
#include <string>

namespace mpc {

	namespace sampler {
		class Sampler;
	}

	namespace file {
		namespace aps {

			class ApsSoundNames
			{

			public:
				static const int NAME_STRING_LENGTH{ 16 };
				std::vector<std::string> names{};
				std::vector<char> saveBytes{};

			public:
				std::vector<std::string> get();
				std::vector<char> getBytes();

				ApsSoundNames(std::vector<char> loadBytes);
				ApsSoundNames(mpc::sampler::Sampler* sampler);
			};
		}
	}
}
