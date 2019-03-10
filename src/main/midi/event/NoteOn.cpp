#include <midi/event/NoteOn.hpp>

#include <midi/event/ChannelEvent.hpp>

using namespace mpc::midi::event;

NoteOn::NoteOn(int tick, int channel, int note, int velocity) : ChannelEvent(tick, ChannelEvent::NOTE_ON, channel, note, velocity)
{
}

NoteOn::NoteOn(int tick, int delta, int channel, int note, int velocity) : ChannelEvent(tick, delta, ChannelEvent::NOTE_ON, channel, note, velocity)
{
}

int NoteOn::getNoteValue()
{
    return mValue1;
}

int NoteOn::getVelocity()
{
    return mValue2;
}

void NoteOn::setNoteValue(int p)
{
    mValue1 = p;
}

void NoteOn::setVelocity(int v)
{
    mValue2 = v;
}
