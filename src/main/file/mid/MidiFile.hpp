#pragma once

#include <ostream>
#include <istream>
#include <vector>
#include <memory>

namespace mpc::file::mid
{
    class MidiTrack;
}

namespace mpc::file::mid
{

    class MidiFile
    {

    public:
        static const int HEADER_SIZE{14};

    private:
        static std::vector<char> IDENTIFIER;

        int mType = 0;
        int mTrackCount = 0;
        int mResolution = 96;
        std::vector<std::shared_ptr<MidiTrack>> mTracks;

    public:
        void setType(int type);
        int getType() const;

        int getLengthInTicks() const;
        std::vector<std::weak_ptr<MidiTrack>> getTracks() const;
        void addTrack(const std::shared_ptr<MidiTrack> &T);
        void addTrack(std::shared_ptr<MidiTrack> T, int pos);

        void writeToOutputStream(const std::shared_ptr<std::ostream> &) const;

    private:
        void initFromBuffer(std::vector<char> &buffer);

    public:
        MidiFile() = default;
        explicit MidiFile(std::shared_ptr<std::istream>);
    };
} // namespace mpc::file::mid
