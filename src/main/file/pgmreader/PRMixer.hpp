#pragma once

#include <vector>

namespace mpc
{
    namespace file
    {
        namespace pgmreader
        {

            class ProgramFileReader;

            class Mixer
            {

            public:
                std::vector<char> mixerArray{};

            private:
                ProgramFileReader *programFile{};

            public:
                int getSampleNamesSize() const;
                int getMixerStart();
                int getMixerEnd();
                std::vector<char> getMixerArray();

            public:
                int getEffectsOutput(int note);
                int getVolume(int note);
                int getPan(int note);
                int getVolumeIndividual(int note);
                int getOutput(int note);
                int getEffectsSendLevel(int note);

                Mixer(ProgramFileReader *programFile);
            };

        } // namespace pgmreader
    } // namespace file
} // namespace mpc
