#include <midi/event/NoteOff.hpp>

#include <midi/event/ChannelEvent.hpp>

using namespace mpc::midi::event;

NoteOff::NoteOff(int tick, int channel, int note, int velocity) : ChannelEvent(tick, ChannelEvent::NOTE_OFF, channel, note, velocity)
{
}

NoteOff::NoteOff(int tick, int delta, int channel, int note, int velocity) : ChannelEvent(tick, delta, ChannelEvent::NOTE_OFF, channel, note, velocity)
{
}

int NoteOff::getNoteValue()
{
    return mValue1;
}

int NoteOff::getVelocity()
{
    return mValue2;
}

void NoteOff::setNoteValue(int p)
{
    mValue1 = p;
}

void NoteOff::setVelocity(int v)
{
    mValue2 = v;
}
