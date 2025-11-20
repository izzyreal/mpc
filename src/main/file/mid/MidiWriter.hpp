#pragma once

#include "file/mid/event/NoteOn.hpp"
#include "file/mid/event/NoteOff.hpp"
#include "file/mid/MidiFile.hpp"
#include "file/mid/MidiTrack.hpp"

#include <memory>

namespace mpc::sequencer
{
    class Sequence;
}

namespace mpc::file::mid
{
    class MidiWriter
    {
    public:
        MidiWriter(sequencer::Sequence *sequence);
        void writeToOStream(const std::shared_ptr<std::ostream> &) const;

    private:
        std::vector<std::shared_ptr<event::NoteOn>> noteOffs;
        std::vector<std::shared_ptr<event::NoteOff>> variations;
        std::vector<std::shared_ptr<event::NoteOn>> noteOns;
        std::vector<std::shared_ptr<event::MidiEvent>> miscEvents;
        sequencer::Sequence *sequence;
        std::unique_ptr<MidiFile> mf;

        void addNoteOn(const std::shared_ptr<event::NoteOn> &noteOn);
        void createDeltas(const std::weak_ptr<MidiTrack> &midiTrack) const;
    };
} // namespace mpc::file::mid
