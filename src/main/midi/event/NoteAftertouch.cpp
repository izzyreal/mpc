#include <midi/event/NoteAftertouch.hpp>

#include <midi/event/ChannelEvent.hpp>

using namespace mpc::midi::event;

NoteAftertouch::NoteAftertouch(int tick, int channel, int note, int amount) : ChannelEvent(tick, ChannelEvent::NOTE_AFTERTOUCH, channel, note, amount)
{
}

NoteAftertouch::NoteAftertouch(int tick, int delta, int channel, int note, int amount) : ChannelEvent(tick, delta, ChannelEvent::NOTE_AFTERTOUCH, channel, note, amount)
{
}

int NoteAftertouch::getNoteValue()
{
    return mValue1;
}

int NoteAftertouch::getAmount()
{
    return mValue2;
}

void NoteAftertouch::setNoteValue(int p)
{
    mValue1 = p;
}

void NoteAftertouch::setAmount(int a)
{
    mValue2 = a;
}
