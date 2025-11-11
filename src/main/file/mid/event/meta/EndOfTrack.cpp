#include "file/mid/event/meta/EndOfTrack.hpp"

#include "file/mid/event/MidiEvent.hpp"
#include "file/mid/event/meta/MetaEvent.hpp"
#include "file/mid/util/VariableLengthInt.hpp"

using namespace mpc::file::mid::event::meta;

EndOfTrack::EndOfTrack(int tick, int delta)
    : MetaEvent(tick, delta, END_OF_TRACK)
{
    mLength = util::VariableLengthInt(0);
}

int EndOfTrack::getEventSize()
{
    return 3;
}

void EndOfTrack::writeToOutputStream(std::ostream &out)
{
    MetaEvent::writeToOutputStream(out);
    out << (char)0x00;
}

void EndOfTrack::writeToOutputStream(std::ostream &out, bool writeType)
{
    MetaEvent::writeToOutputStream(out, writeType);
}

int EndOfTrack::compareTo(MidiEvent *other) const
{
    if (mTick != other->getTick())
    {
        return mTick < other->getTick() ? -1 : 1;
    }
    if (mDelta.getValue() != other->getDelta())
    {
        return mDelta.getValue() < other->getDelta() ? 1 : -1;
    }
    if (dynamic_cast<EndOfTrack *>(other) == nullptr)
    {
        return 1;
    }
    return 0;
}
