#include "file/mid/event/NoteOn.hpp"

#include "file/mid/event/ChannelEvent.hpp"

using namespace mpc::file::mid::event;

NoteOn::NoteOn(int tick, int channel, int note, int velocity)
    : ChannelEvent(tick, NOTE_ON, channel, note, velocity)
{
}

NoteOn::NoteOn(int tick, int delta, int channel, int note, int velocity)
    : ChannelEvent(tick, delta, NOTE_ON, channel, note, velocity)
{
}

int NoteOn::getNoteValue() const
{
    return mValue1;
}

int NoteOn::getVelocity() const
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
