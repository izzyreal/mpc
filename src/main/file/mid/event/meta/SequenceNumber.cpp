#include "file/mid/event/meta/SequenceNumber.hpp"

#include "file/mid/event/MidiEvent.hpp"
#include "file/mid/event/meta/GenericMetaEvent.hpp"
#include "file/mid/event/meta/MetaEventData.hpp"
#include "file/mid/event/meta/MetaEvent.hpp"
#include "file/mid/util/VariableLengthInt.hpp"

using namespace mpc::file::mid::event::meta;

SequenceNumber::SequenceNumber(int tick, int delta, int number)
    : MetaEvent(tick, delta, SEQUENCE_NUMBER)
{
    mNumber = number;
    mLength = util::VariableLengthInt(2);
}

int SequenceNumber::getMostSignificantBits() const
{
    return mNumber >> 8;
}

int SequenceNumber::getLeastSignificantBits() const
{
    return mNumber & 255;
}

int SequenceNumber::getSequenceNumber() const
{
    return mNumber;
}

void SequenceNumber::writeToOutputStream(std::ostream &out)
{
    MetaEvent::writeToOutputStream(out);
    out << (char)2;
    out << (char)getMostSignificantBits();
    out << (char)getLeastSignificantBits();
}

void SequenceNumber::writeToOutputStream(std::ostream &out, bool writeType)
{
    MetaEvent::writeToOutputStream(out, writeType);
}

std::shared_ptr<MetaEvent>
SequenceNumber::parseSequenceNumber(int tick, int delta, MetaEventData *info)
{
    if (info->length.getValue() != 2)
    {
        return std::make_shared<GenericMetaEvent>(tick, delta, info);
    }
    int msb = info->data[0];
    int lsb = info->data[1];
    auto number = (msb << 8) + lsb;
    return std::make_shared<SequenceNumber>(tick, delta, number);
}

int SequenceNumber::getEventSize()
{
    return 5;
}

int SequenceNumber::compareTo(MidiEvent *other) const
{
    if (mTick != other->getTick())
    {
        return mTick < other->getTick() ? -1 : 1;
    }
    if (mDelta.getValue() != other->getDelta())
    {
        return mDelta.getValue() < other->getDelta() ? 1 : -1;
    }
    if (dynamic_cast<SequenceNumber *>(other) == nullptr)
    {
        return 1;
    }
    auto o = dynamic_cast<SequenceNumber *>(other);
    if (mNumber != o->mNumber)
    {
        return mNumber < o->mNumber ? -1 : 1;
    }
    return 0;
}
