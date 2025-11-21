#pragma once

#include "sequencer/EventState.hpp"

#include <memory>
#include <vector>

namespace mpc::sequencer
{
    struct TrackEventState;

    class TrackEventStateView
    {
    public:
        explicit TrackEventStateView(
            const std::shared_ptr<const TrackEventState> &s) noexcept;

        std::optional<EventState> findNoteEvent(int tick,
                                                NoteNumber note) const;

        EventState getEventByIndex(EventIndex) const;

        std::vector<EventState> getEventRange(int startTick, int endTick) const;

        bool isEventsEmpty() const;

        int getEventCount() const;

        EventIndex getEventIndex() const;

        std::vector<EventState> getNoteEvents() const;

        EventState findRecordingNoteOnByNoteNumber(NoteNumber) const;

        EventState findRecordingNoteOnByNoteEventId(NoteEventId) const;

    private:
        const std::shared_ptr<const TrackEventState> state;
    };
} // namespace mpc::sequencer
