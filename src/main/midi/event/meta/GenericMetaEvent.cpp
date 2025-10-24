#include "midi/event/meta/GenericMetaEvent.hpp"

#include "midi/event/MidiEvent.hpp"
#include "midi/event/meta/MetaEventData.hpp"
#include "midi/util/VariableLengthInt.hpp"

using namespace mpc::midi::event::meta;

GenericMetaEvent::GenericMetaEvent(int tick, int delta, MetaEventData* info) 
	: MetaEvent(tick, delta, info->type)
{
	mData = info->data;
	mLength = info->length;
}

int GenericMetaEvent::getEventSize()
{
	return 1 + 1 + mLength.getByteCount() + mLength.getValue();
}

void GenericMetaEvent::writeToOutputStream(std::ostream& out)
{
	MetaEvent::writeToOutputStream(out);
	auto length = mLength.getBytes();
	out.write(&length[0], length.size());
	out.write(&mData[0], mData.size());
}

void GenericMetaEvent::writeToOutputStream(std::ostream& out, bool writeType)
{
	MetaEvent::writeToOutputStream(out, writeType);
}

int GenericMetaEvent::compareTo(mpc::midi::event::MidiEvent* other)
{
	if (mTick != other->getTick()) {
		return mTick < other->getTick() ? -1 : 1;
	}
	if (mDelta.getValue() != other->getDelta()) {
		return mDelta.getValue() < other->getDelta() ? 1 : -1;
	}
	return 1;
}
