#pragma once

#include "file/mid/MidiFile.hpp"

#include "sequencer/EventData.hpp"

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
                   const std::weak_ptr<sequencer::Sequence> &dest);
        void parseSequence(Mpc &) const;

    private:
        static bool isInteger(const std::string &);

        std::unique_ptr<MidiFile> midiFile;
        std::weak_ptr<sequencer::Sequence> dest;

        static int getNumberOfNoteOns(
            int noteValue,
            const std::vector<std::shared_ptr<event::ChannelEvent>> &allNotes);
        static int
        getNumberOfNotes(int noteValue,
                         const std::vector<sequencer::EventData> &allNotes);
    };
} // namespace mpc::file::mid
