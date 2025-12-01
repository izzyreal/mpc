#pragma once

#include "IntTypes.hpp"
#include "sequencer/BusType.hpp"

#include <vector>

namespace mpc::sequencer
{
    struct EventData;
    struct TrackState;

    class TrackStateView
    {
    public:
        explicit TrackStateView(const TrackState &s) noexcept;

        EventData *findNoteEvent(int tick, NoteNumber note) const;

        EventData *getEventByIndex(EventIndex) const;

        std::vector<EventData *> getEventRange(int startTick,
                                                int endTick) const;

        bool isEventsEmpty() const;

        int getEventCount() const;

        std::vector<EventData *> getNoteEvents() const;

        std::vector<EventData *> getEvents() const;

        EventData *findRecordingNoteOnByNoteNumber(NoteNumber) const;

        EventData *findRecordingNoteOn(const EventData *) const;

        EventData *getEvent(const EventData *) const;

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
