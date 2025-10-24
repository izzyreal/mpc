#pragma once

#include <vector>

namespace mpc
{
    namespace file
    {
        namespace pgmreader
        {

            class ProgramFileReader;

            class Slider
            {

            public:
                int sampleNamesSize{0};

            public:
                std::vector<char> sliderArray{};

            private:
                ProgramFileReader *programFile{};

            public:
                int getSampleNamesSize();
                int getSliderStart();
                int getSliderEnd();
                std::vector<char> getSliderArray();

            public:
                int getMidiNoteAssign();
                int getTuneLow();
                int getTuneHigh();
                int getDecayLow();
                int getDecayHigh();
                int getAttackLow();
                int getAttackHigh();
                int getFilterLow();
                int getFilterHigh();
                int getControlChange();

                Slider(ProgramFileReader *programFile);
            };

        } // namespace pgmreader
    } // namespace file
} // namespace mpc
