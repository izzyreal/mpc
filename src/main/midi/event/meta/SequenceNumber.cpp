#include <midi/event/meta/SequenceNumber.hpp>

#include <midi/event/MidiEvent.hpp>
#include <midi/event/meta/GenericMetaEvent.hpp>
#include <midi/event/meta/MetaEventData.hpp>
#include <midi/event/meta/MetaEvent.hpp>
#include <midi/util/VariableLengthInt.hpp>

using namespace mpc::midi::event::meta;
using namespace std;

SequenceNumber::SequenceNumber(int tick, int delta, int number)
	: MetaEvent(tick, delta, MetaEvent::SEQUENCE_NUMBER, mpc::midi::util::VariableLengthInt(2))
{
	mNumber = number;
}

int SequenceNumber::getMostSignificantBits()
{
    return mNumber >> 8;
}

int SequenceNumber::getLeastSignificantBits()
{
    return mNumber & 255;
}

int SequenceNumber::getSequenceNumber()
{
    return mNumber;
}

void SequenceNumber::writeToOutputStream(ostream& out) 
{
    MetaEvent::writeToOutputStream(out);
    out << 2;
    out << getMostSignificantBits();
    out << getLeastSignificantBits();
}

void SequenceNumber::writeToOutputStream(ostream& out, bool writeType)
{
    MetaEvent::writeToOutputStream(out, writeType);
}

shared_ptr<MetaEvent> SequenceNumber::parseSequenceNumber(int tick, int delta, MetaEventData* info)
{
	if (info->length.getValue() != 2) {
		return make_shared<GenericMetaEvent>(tick, delta, info);
	}
	int msb = info->data[0];
	int lsb = info->data[1];
	auto number = (msb << 8) + lsb;
	return make_shared<SequenceNumber>(tick, delta, number);
}

int SequenceNumber::getEventSize()
{
    return 5;
}

int SequenceNumber::compareTo(mpc::midi::event::MidiEvent* other)
{
    if(mTick != other->getTick()) {
        return mTick < other->getTick() ? -1 : 1;
    }
    if(mDelta.getValue() != other->getDelta()) {
        return mDelta.getValue() < other->getDelta() ? 1 : -1;
    }
    if(!(dynamic_cast< SequenceNumber* >(other) != nullptr)) {
        return 1;
    }
    auto o = dynamic_cast< SequenceNumber* >(other);
    if(mNumber != o->mNumber) {
        return mNumber < o->mNumber ? -1 : 1;
    }
    return 0;
}

