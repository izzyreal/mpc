#pragma once

#include "IntTypes.hpp"
#include "sequencer/Event.hpp"

namespace mpc::sequencer
{
    class NoteOffEvent final : public Event
    {
    public:
        explicit NoteOffEvent(
            const std::function<EventState()> &getSnapshot,
            const std::function<void(TrackEventMessage &&)> &dispatch,
            NoteNumber);

        explicit NoteOffEvent(
            const std::function<EventState()> &getSnapshot,
            const std::function<void(TrackEventMessage &&)> &dispatch);

        void setNote(NoteNumber);

        NoteNumber getNote() const;

        std::string getTypeName() const override
        {
            return "note-off";
        }
    };
} // namespace mpc::sequencer
