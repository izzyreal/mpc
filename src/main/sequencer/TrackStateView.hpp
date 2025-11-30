#pragma once

#include "IntTypes.hpp"
#include "sequencer/BusType.hpp"

#include <vector>

namespace mpc::sequencer
{
    struct EventState;
    struct TrackState;

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

        EventIndex getPlayEventIndex() const;

        uint8_t getVelocityRatio() const;

        uint8_t getProgramChange() const;

        uint8_t getDeviceIndex() const;

        BusType getBusType() const;

        bool isOn() const;

        bool isUsed() const;

    private:
        const TrackState &state;
    };
} // namespace mpc::sequencer
