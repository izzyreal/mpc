#pragma once
#include "sequencer/Event.hpp"

namespace mpc::sequencer
{
    class MidiClockEvent : public Event
    {

    public:
        typedef Event super;

    private:
        int status{0};

    public:
        virtual int getStatus() const;
        virtual void setStatus(int i);

        MidiClockEvent(int statusToUse);
        MidiClockEvent(const MidiClockEvent &);

        std::string getTypeName() const override
        {
            return "midi-clock";
        }
    };
} // namespace mpc::sequencer
