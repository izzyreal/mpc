#pragma once

#include "eventregistry/EventTypes.hpp"

#include <vector>
#include <memory>

namespace mpc::eventregistry
{
    struct Snapshot
    {
        std::vector<PhysicalPadPressEventPtr> physicalPadEvents;
        std::vector<ProgramPadEventPtr>  programPadEvents;
        std::vector<NoteEventPtr>           noteEvents;
    };

    class SnapshotView
    {
    public:
        explicit SnapshotView(const Snapshot *s) noexcept : snap(s) {}

        bool isProgramPadPressedBySource(ProgramPadIndex idx, Source src) const;
        VelocityOrPressure
        getPressedProgramPadAfterTouchOrVelocity(ProgramPadIndex idx) const;
        bool isProgramPadPressed(ProgramPadIndex idx) const;
        int getTotalPressedProgramPadCount() const;

        NoteEventPtr retrievePlayNoteEvent(NoteNumber note) const;
        std::shared_ptr<sequencer::NoteOnEvent>
        retrieveRecordNoteEvent(NoteNumber note) const;

        PhysicalPadPressEventPtr
        retrievePhysicalPadPressEvent(PhysicalPadIndex idx) const;

        NoteEventPtr retrieveNoteEvent(NoteNumber note, Source src) const;

        bool valid() const noexcept
        {
            return snap != nullptr;
        }

        void printStats()
        {
            printf("===== EventRegistry Stats =======\n");
            printf("physicalPadPressEvent count: %zu\n",
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
}
