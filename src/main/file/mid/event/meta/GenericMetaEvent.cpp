#include "file/mid/event/meta/GenericMetaEvent.hpp"

#include "file/mid/event/MidiEvent.hpp"
#include "file/mid/event/meta/MetaEventData.hpp"
#include "file/mid/util/VariableLengthInt.hpp"

using namespace mpc::file::mid::event::meta;

GenericMetaEvent::GenericMetaEvent(int tick, int delta, MetaEventData *info)
    : MetaEvent(tick, delta, info->type)
{
    mData = info->data;
    mLength = info->length;
}

int GenericMetaEvent::getEventSize()
{
    return 1 + 1 + mLength.getByteCount() + mLength.getValue();
}

void GenericMetaEvent::writeToOutputStream(std::ostream &out)
{
    MetaEvent::writeToOutputStream(out);
    auto length = mLength.getBytes();
    out.write(&length[0], length.size());
    out.write(&mData[0], mData.size());
}

void GenericMetaEvent::writeToOutputStream(std::ostream &out, bool writeType)
{
    MetaEvent::writeToOutputStream(out, writeType);
}

int GenericMetaEvent::compareTo(MidiEvent *other)
{
    if (mTick != other->getTick())
    {
        return mTick < other->getTick() ? -1 : 1;
    }
    if (mDelta.getValue() != other->getDelta())
    {
        return mDelta.getValue() < other->getDelta() ? 1 : -1;
    }
    return 1;
}
