#pragma once
#include "IntTypes.hpp"
#include "sequencer/EventRef.hpp"

namespace mpc::sequencer
{
    class NoteOnEvent final : public EventRef
    {
    public:
        explicit NoteOnEvent(
            EventData *ptr, const EventData &snapshot,
            const std::function<void(TrackMessage &&)> &dispatch, NoteNumber,
            Velocity vel = MaxVelocity);
        explicit NoteOnEvent(
            EventData *ptr, const EventData &snapshot,
            const std::function<void(TrackMessage &&)> &dispatch);
        explicit NoteOnEvent(
            EventData *ptr, const EventData &snapshot,
            const std::function<void(TrackMessage &&)> &dispatch,
            DrumNoteNumber);

        void setNote(NoteNumber) const;
        NoteNumber getNote() const;
        void setDuration(Duration duration) const;
        Duration getDuration() const;
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
