#pragma once

#include <memory>
#include <vector>
#include <string>

namespace mpc
{

    namespace sampler
    {
        class Sampler;
        class Program;
    } // namespace sampler

    namespace file
    {
        namespace pgmwriter
        {

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
                SampleNames(mpc::sampler::Program *program,
                            std::weak_ptr<mpc::sampler::Sampler> sampler);
            };

        } // namespace pgmwriter
    } // namespace file
} // namespace mpc
