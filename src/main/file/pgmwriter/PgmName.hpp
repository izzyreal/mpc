#pragma once

#include <vector>

namespace mpc
{

    namespace sampler
    {
        class Program;
    }

    namespace file
    {
        namespace pgmwriter
        {

            class PgmName
            {

            public:
                int sampleNamesSize{0};

                std::vector<char> programNameArray{};

                std::vector<char> getPgmNameArray();

                PgmName(sampler::Program *program);
            };

        } // namespace pgmwriter
    } // namespace file
} // namespace mpc
