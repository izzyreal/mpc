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

        std::optional<std::pair<EventId, EventState>>
        findNoteEvent(int tick, NoteNumber note) const;

        EventState getEventByIndex(EventIndex) const;

        std::vector<std::pair<EventId, EventState>>
        getEventRange(int startTick, int endTick) const;

        bool isEventsEmpty() const;

        int getEventCount() const;

        std::vector<std::pair<EventId, EventState>> getNoteEvents() const;

        std::pair<EventId, EventState>
            findRecordingNoteOnByNoteNumber(NoteNumber) const;

        std::pair<EventId, EventState>
            findRecordingNoteOnByNoteEventId(NoteEventId) const;

        std::pair<EventId, EventState> getEventById(EventId) const;

    private:
        const std::shared_ptr<const TrackEventState> state;
    };
} // namespace mpc::sequencer
