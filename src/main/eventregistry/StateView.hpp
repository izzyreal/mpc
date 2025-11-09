#pragma once

#include "eventregistry/EventTypes.hpp"

#include <memory>

namespace mpc::eventregistry
{
    struct State;

    class StateView
    {
    public:
        explicit StateView(const std::shared_ptr<const State> s) noexcept
            : state(s)
        {
        }

        bool isProgramPadPressedBySource(ProgramPadIndex idx, Source src) const;
        VelocityOrPressure
        getPressedProgramPadAfterTouchOrVelocity(ProgramPadIndex idx) const;

        bool isProgramPadPressed(ProgramPadIndex idx) const;
        bool
        isProgramPadPressed(ProgramPadIndex idx,
                            const std::shared_ptr<sampler::Program> &) const;

        ProgramPadPressEventPtr getMostRecentProgramPadPress(
            ProgramPadIndex idx,
            const ::std::vector<Source> &sourcesToExclude) const;

        int getTotalPressedProgramPadCount() const;

        int getTotalNoteOnCount() const;

        NoteOnEventPtr retrievePlayNoteEvent(NoteNumber note) const;
        std::shared_ptr<sequencer::NoteOnEvent>
        retrieveRecordNoteEvent(NoteNumber note) const;

        PhysicalPadPressEventPtr
        retrievePhysicalPadPressEvent(PhysicalPadIndex idx) const;

        NoteOnEventPtr retrieveNoteEvent(NoteNumber note, Source src) const;

        bool valid() const noexcept
        {
            return state != nullptr;
        }

    private:
        std::shared_ptr<const State> state;
    };
} // namespace mpc::eventregistry
