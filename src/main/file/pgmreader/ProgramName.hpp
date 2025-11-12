#pragma once

#include <vector>
#include <string>

namespace mpc
{
    namespace file
    {
        namespace pgmreader
        {

            class ProgramFileReader;

            class ProgramName
            {

            public:
                int sampleNamesSize{0};

                std::vector<char> programNameArray{};

            private:
                ProgramFileReader *programFile{};

            public:
                int getSampleNamesSize();
                int getProgramNameStart();
                int getProgramNameEnd();
                std::vector<char> getProgramNameArray();

                std::string getProgramNameASCII();

                ProgramName(ProgramFileReader *programFile);
            };

        } // namespace pgmreader
    } // namespace file
} // namespace mpc
