#pragma once

#include "SequencerState.hpp"
#include "sequencer/EventState.hpp"

#include <vector>

namespace mpc::sequencer
{
    struct SequencerState;

    class TrackStateView
    {
    public:
        explicit TrackStateView(const TrackState &s) noexcept;

        EventState *findNoteEvent(int tick, NoteNumber note) const;

        EventState *getEventByIndex(EventIndex) const;

        std::vector<EventState *> getEventRange(int startTick,
                                                int endTick) const;

        bool isEventsEmpty() const;

        int getEventCount() const;

        std::vector<EventState *> getNoteEvents() const;

        std::vector<EventState *> getEvents() const;

        EventState *findRecordingNoteOnByNoteNumber(NoteNumber) const;

        EventState *findRecordingNoteOn(const EventState *) const;

        EventState *getEvent(const EventState *) const;

    private:
        const TrackState &state;
    };
} // namespace mpc::sequencer
