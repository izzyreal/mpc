#pragma once
#include "IntTypes.hpp"
#include "sequencer/Event.hpp"

namespace mpc::sequencer
{
    class NoteOnEvent final : public Event
    {
    public:
        explicit NoteOnEvent(
            const std::function<EventState()>
                &getSnapshot,
            const std::function<void(TrackEventMessage &&)> &dispatch,
            NoteNumber, Velocity vel = MaxVelocity);
        explicit NoteOnEvent(
            const std::function<EventState()>
                &getSnapshot,
            const std::function<void(TrackEventMessage &&)> &dispatch);
        explicit NoteOnEvent(
            const std::function<EventState()>
                &getSnapshot,
            const std::function<void(TrackEventMessage &&)> &dispatch,
            DrumNoteNumber);

        void setNote(NoteNumber) const;
        NoteNumber getNote() const;
        void setDuration(Duration duration) const;
        Duration getDuration() const;
        void resetDuration() const;
        NoteVariationType getVariationType() const;
        void setVariationType(NoteVariationType) const;
        void setVariationValue(int i) const;
        int getVariationValue() const;
        void setVelocity(Velocity) const;
        Velocity getVelocity() const;

        std::string getTypeName() const override
        {
            return "note-on";
        }
    };
} // namespace mpc::sequencer
