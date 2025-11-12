#pragma once
#include "Util.hpp"

#include <string>
#include <vector>
#include <memory>

namespace mpc::disk
{
    class MpcFile;
}

namespace mpc::file::sndreader
{
    class SndHeaderReader;
}

namespace mpc::file::sndreader
{
    class SndReader
    {

        std::vector<char> sndFileArray;
        std::shared_ptr<SndHeaderReader> sndHeaderReader;

    public:
        std::string getName() const;
        bool isHeaderValid() const;
        bool isMono() const;

        int getSampleRate() const;
        int getLevel() const;
        int getStart() const;
        int getEnd() const;
        int getLoopLength() const;
        bool isLoopEnabled() const;
        int getTune() const;
        int getNumberOfBeats() const;
        void readData(const std::shared_ptr<std::vector<float>> &);

        std::vector<char> &getSndFileArray();

        SndReader(disk::MpcFile *);

        SndReader(const std::vector<char> &loadBytes);
    };
} // namespace mpc::file::sndreader
