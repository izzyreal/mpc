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

                std::vector<char> sampleNamesArray{};
                int numberOfSamples{};
                std::vector<int> snConvTable{};

            public:
                std::vector<char> getSampleNamesArray();

            private:
                void setSampleName(int sampleNumber, const std::string &name);

            public:
                int getNumberOfSamples() const;
                std::vector<int> getSnConvTable();

                SampleNames(sampler::Program *program,
                            const std::weak_ptr<sampler::Sampler> &sampler);
            };

        } // namespace pgmwriter
    } // namespace file
} // namespace mpc
