#pragma once

#include "file/mid/MidiFile.hpp"

#include <memory>
#include <string>
#include <vector>

namespace mpc
{
    class Mpc;
}

namespace mpc::file::mid::event
{
    class ChannelEvent;
}

namespace mpc::sequencer
{
    class Sequence;
    class NoteOnEvent;
} // namespace mpc::sequencer

namespace mpc::disk
{
    class MpcFile;
}

namespace mpc::file::mid
{
    class MidiReader
    {
    public:
        MidiReader(std::shared_ptr<std::istream>,
                   std::weak_ptr<mpc::sequencer::Sequence> dest);
        void parseSequence(mpc::Mpc &);

    private:
        static bool isInteger(std::string);

        std::unique_ptr<mpc::file::mid::MidiFile> midiFile;
        std::weak_ptr<mpc::sequencer::Sequence> dest;

        int getNumberOfNoteOns(
            int noteValue,
            std::vector<std::shared_ptr<mpc::file::mid::event::ChannelEvent>>
                allNotes);
        int getNumberOfNotes(
            int noteValue,
            std::vector<std::shared_ptr<mpc::sequencer::NoteOnEvent>> allNotes);
    };
} // namespace mpc::file::mid
