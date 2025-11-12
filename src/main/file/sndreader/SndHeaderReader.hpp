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
        std::vector<char> headerArray = std::vector<char>(42);

    public:
        std::vector<char> &getHeaderArray();
        bool hasValidId() const;
        std::string getName() const;
        int getLevel() const;
        int getTune() const;
        bool isMono() const;
        int getStart() const;
        int getEnd() const;
        int getNumberOfFrames() const;
        int getLoopLength() const;
        bool isLoopEnabled() const;
        int getNumberOfBeats() const;
        int getSampleRate() const;

        SndHeaderReader(SndReader *sndReader);
    };
} // namespace mpc::file::sndreader
