#pragma once

#include <string>
#include <vector>

namespace mpc::file::sndreader
{
    class SndReader;
}

namespace mpc::file::sndreader
{
    class SndHeaderReader
    {
    private:
        std::vector<char> headerArray = std::vector<char>(42);

    public:
        std::vector<char> &getHeaderArray();
        bool hasValidId();
        std::string getName();
        int getLevel();
        int getTune();
        bool isMono();
        int getStart();
        int getEnd();
        int getNumberOfFrames();
        int getLoopLength();
        bool isLoopEnabled();
        int getNumberOfBeats();
        int getSampleRate();

    public:
        SndHeaderReader(SndReader *sndReader);
    };
} // namespace mpc::file::sndreader
