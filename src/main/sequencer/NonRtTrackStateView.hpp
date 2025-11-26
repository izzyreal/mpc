#pragma once

#include "NonRtSequencerState.hpp"
#include "PlaybackState.hpp"
#include "sequencer/EventState.hpp"

#include <memory>
#include <vector>

namespace mpc::sequencer
{
    struct NonRtSequencerState;

    class NonRtTrackStateView
    {
    public:
        explicit NonRtTrackStateView(
            const NonRtTrackState &s) noexcept;

        std::optional<EventState> findNoteEvent(int tick,
                                                NoteNumber note) const;

        EventState getEventByIndex(EventIndex) const;

        std::vector<EventState> getEventRange(int startTick, int endTick) const;

        bool isEventsEmpty() const;

        int getEventCount() const;

        std::vector<EventState> getNoteEvents() const;

        std::vector<EventState> getEvents() const;

        EventState findRecordingNoteOnByNoteNumber(NoteNumber) const;

        EventState findRecordingNoteOnByNoteEventId(NoteEventId) const;

        EventState getEventById(EventId) const;

    private:
        const NonRtTrackState &state;
    };
} // namespace mpc::sequencer
