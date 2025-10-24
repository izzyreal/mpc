#pragma once

#include <ostream>
#include <istream>
#include <vector>
#include <memory>

namespace mpc::midi
{
    class MidiTrack;
}

namespace mpc::midi
{

    class MidiFile
    {

    public:
        static const int HEADER_SIZE{14};

    private:
        static std::vector<char> IDENTIFIER;

    private:
        int mType = 0;
        int mTrackCount = 0;
        int mResolution = 96;
        std::vector<std::shared_ptr<MidiTrack>> mTracks;

    public:
        void setType(int type);
        int getType();

        int getLengthInTicks();
        std::vector<std::weak_ptr<MidiTrack>> getTracks();
        void addTrack(std::shared_ptr<MidiTrack> T);
        void addTrack(std::shared_ptr<MidiTrack> T, int pos);

        void writeToOutputStream(std::shared_ptr<std::ostream>);

    private:
        void initFromBuffer(std::vector<char> &buffer);

    public:
        MidiFile() = default;
        explicit MidiFile(std::shared_ptr<std::istream>);
    };
} // namespace mpc::midi
