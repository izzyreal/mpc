#pragma once

#include "eventregistry/EventTypes.hpp"

#include <memory>
#include <cstdio>

namespace mpc::eventregistry
{
    struct Snapshot
    {
        PhysicalPadPressEventPtrs physicalPadEvents;
        ProgramPadPressEventPtrs programPadEvents;
        NoteOnEventPtrs noteEvents;
    };

    class SnapshotView
    {
    public:
        explicit SnapshotView(const Snapshot *s) noexcept : snap(s) {}

        bool isProgramPadPressedBySource(ProgramPadIndex idx, Source src) const;
        VelocityOrPressure
        getPressedProgramPadAfterTouchOrVelocity(ProgramPadIndex idx) const;
        bool isProgramPadPressed(ProgramPadIndex idx) const;
        bool isProgramPadPressed(
            ProgramPadIndex idx,
            const ::std::vector<Source> &sourcesToExclude) const;
        int getTotalPressedProgramPadCount() const;

        NoteOnEventPtr retrievePlayNoteEvent(NoteNumber note) const;
        std::shared_ptr<sequencer::NoteOnEvent>
        retrieveRecordNoteEvent(NoteNumber note) const;

        PhysicalPadPressEventPtr
        retrievePhysicalPadPressEvent(PhysicalPadIndex idx) const;

        NoteOnEventPtr retrieveNoteEvent(NoteNumber note, Source src) const;

        bool valid() const noexcept
        {
            return snap != nullptr;
        }

        void printStats()
        {
            printf("===== EventRegistry Stats =======\n");
            printf("physicalPadEvent count: %zu\n",
                   snap->physicalPadEvents.size());
            printf("programPadEvent  count: %zu\n",
                   snap->programPadEvents.size());
            printf("noteEvent        count: %zu\n", snap->noteEvents.size());
            printf("=================================\n");
        }

    private:
        const Snapshot *snap{};
        friend class EventRegistry;
    };
} // namespace mpc::eventregistry
