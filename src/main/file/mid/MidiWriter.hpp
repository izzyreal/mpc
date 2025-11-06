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
        MidiWriter(mpc::sequencer::Sequence *sequence);
        void writeToOStream(const std::shared_ptr<std::ostream> &) const;

    private:
        std::vector<std::shared_ptr<mpc::file::mid::event::NoteOn>> noteOffs;
        std::vector<std::shared_ptr<mpc::file::mid::event::NoteOff>> variations;
        std::vector<std::shared_ptr<mpc::file::mid::event::NoteOn>> noteOns;
        std::vector<std::shared_ptr<mpc::file::mid::event::MidiEvent>>
            miscEvents;
        mpc::sequencer::Sequence *sequence;
        std::unique_ptr<mpc::file::mid::MidiFile> mf;

    private:
        void
        addNoteOn(const std::shared_ptr<mpc::file::mid::event::NoteOn> &noteOn);
        void
        createDeltas(const std::weak_ptr<mpc::file::mid::MidiTrack> &mt) const;
    };
} // namespace mpc::file::mid
