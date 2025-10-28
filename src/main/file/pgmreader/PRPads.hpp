#pragma once

#include <vector>

namespace mpc
{
    namespace file
    {
        namespace pgmreader
        {

            class ProgramFileReader;

            class Pads
            {

            public:
                int sampleNamesSize{0};

            public:
                std::vector<char> padsArray{};

            private:
                ProgramFileReader *programFile{};

            public:
                int getSampleNamesSize();
                int getPadsStart();
                int getPadsEnd();
                std::vector<char> getPadsArray();

            public:
                int getNote(int pad);

            public:
                Pads(ProgramFileReader *programFile);
            };

        } // namespace pgmreader
    } // namespace file
} // namespace mpc
