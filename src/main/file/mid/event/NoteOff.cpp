#include "file/mid/event/NoteOff.hpp"

#include "file/mid/event/ChannelEvent.hpp"

using namespace mpc::file::mid::event;

NoteOff::NoteOff(int tick, int channel, int note, int velocity)
    : ChannelEvent(tick, NOTE_OFF, channel, note, velocity)
{
}

NoteOff::NoteOff(int tick, int delta, int channel, int note, int velocity)
    : ChannelEvent(tick, delta, NOTE_OFF, channel, note, velocity)
{
}

int NoteOff::getNoteValue() const
{
    return mValue1;
}

int NoteOff::getVelocity() const
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
