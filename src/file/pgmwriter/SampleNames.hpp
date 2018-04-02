#pragma once

#include <memory>
#include <vector>

namespace mpc {

	namespace sampler {
		class Sampler;
		class Program;
	}

	namespace file {
		namespace pgmwriter {

			class SampleNames
			{

			private:
				std::vector<char> sampleNamesArray{};
				int numberOfSamples{};
				std::vector<int> snConvTable{};

			public: 
				std::vector<char> getSampleNamesArray();

			private:
				void setSampleName(int sampleNumber, std::string name);

			public: 
				int getNumberOfSamples();
				std::vector<int> getSnConvTable();

			public:
				SampleNames(mpc::sampler::Program* program, std::weak_ptr<mpc::sampler::Sampler> sampler);
			};

		}
	}
}
