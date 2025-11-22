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

        std::optional<std::pair<EventIndex, EventState>>
        findNoteEvent(int tick, NoteNumber note) const;

        EventState getEventByIndex(EventIndex) const;

        std::vector<std::pair<EventIndex, EventState>>
        getEventRange(int startTick, int endTick) const;

        bool isEventsEmpty() const;

        int getEventCount() const;

        std::vector<std::pair<EventIndex, EventState>> getNoteEvents() const;

        std::pair<EventIndex, EventState>
            findRecordingNoteOnByNoteNumber(NoteNumber) const;

        std::pair<EventIndex, EventState>
            findRecordingNoteOnByNoteEventId(NoteEventId) const;

    private:
        const std::shared_ptr<const TrackEventState> state;
    };
} // namespace mpc::sequencer
