#include "sequencer/MidiClockEvent.hpp"

using namespace mpc::sequencer;

MidiClockEvent::MidiClockEvent(int statusToUse) : status(statusToUse)
{
}

mpc::sequencer::MidiClockEvent::MidiClockEvent(std::shared_ptr<mpc::engine::midi::ShortMessage> msg) : MidiClockEvent(msg->getStatus())
{
}

mpc::sequencer::MidiClockEvent::MidiClockEvent(const MidiClockEvent &event) : Event(event)
{
    setStatus(event.getStatus());
}

int MidiClockEvent::getStatus() const
{
    return status;
}

void MidiClockEvent::setStatus(int i)
{
    status = i;
}
