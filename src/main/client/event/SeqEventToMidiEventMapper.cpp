#include "client/event/SeqEventToMidiEventMapper.hpp"

#include "client/event/ClientMidiEvent.hpp"

#include "sequencer/EventData.hpp"

using namespace mpc::client::event;
using namespace mpc::sequencer;

void SeqEventToMidiEventMapper::mapSeqEventToMidiEvent(const EventData &e, ClientMidiEvent &c)
{
    if (e.type == EventType::NoteOn)
    {
        c.setMessageType(ClientMidiEvent::NOTE_ON);
        c.setNoteNumber(e.noteNumber);
        c.setVelocity(e.velocity);
    }
    else if (e.type == EventType::NoteOff)
    {
        c.setMessageType(ClientMidiEvent::NOTE_OFF);
        c.setNoteNumber(e.noteNumber);
    }
}
