#pragma once

#include <vector>

namespace mpc {

	namespace sampler {
		class Program;
	}

	namespace file {
		namespace pgmwriter {

			class PgmName
			{

			public:
				int sampleNamesSize{ 0 };

			public:
				std::vector<char> programNameArray{};

			public:
				std::vector<char> getPgmNameArray();

			public:
				PgmName(mpc::sampler::Program* program);
			};

		}
	}
}
