#include <midi/event/meta/MidiChannelPrefix.hpp>

#include <midi/event/MidiEvent.hpp>
#include <midi/event/meta/GenericMetaEvent.hpp>
#include <midi/event/meta/MetaEventData.hpp>
#include <midi/event/meta/MetaEvent.hpp>
#include <midi/util/VariableLengthInt.hpp>

using namespace mpc::midi::event::meta;
using namespace std;

MidiChannelPrefix::MidiChannelPrefix(int tick, int delta, int channel)
	: MetaEvent(tick, delta, MetaEvent::MIDI_CHANNEL_PREFIX, new mpc::midi::util::VariableLengthInt(4))
{
	mChannel = channel;
}

void MidiChannelPrefix::setChannel(int c)
{
    mChannel = c;
}

int MidiChannelPrefix::getChannel()
{
    return mChannel;
}

int MidiChannelPrefix::getEventSize()
{
    return 4;
}

void MidiChannelPrefix::writeToFile(moduru::io::OutputStream* out) 
{
    MetaEvent::writeToFile(out);
    out->write(1);
    out->write(mChannel);
}

shared_ptr<MetaEvent> MidiChannelPrefix::parseMidiChannelPrefix(int tick, int delta, MetaEventData* info)
{
	if (info->length->getValue() != 1) {
		return make_shared<GenericMetaEvent>(tick, delta, info);
	}
	int channel = info->data[0];
	return make_shared<MidiChannelPrefix>(tick, delta, channel);
}

int MidiChannelPrefix::compareTo(mpc::midi::event::MidiEvent* other)
{
	/*
	if(mTick != npc(other)->getTick()) {
        return mTick < npc(other)->getTick() ? -1 : 1;
    }
    if(npc(mDelta)->getValue() != npc(other)->getDelta()) {
        return npc(mDelta)->getValue() < npc(other)->getDelta() ? 1 : -1;
    }
    if(!(dynamic_cast< MidiChannelPrefix* >(other) != nullptr)) {
        return 1;
    }
    auto o = java_cast< MidiChannelPrefix* >(other);
    if(mChannel != npc(o)->mChannel) {
        return mChannel < npc(o)->mChannel ? -1 : 1;
    }
	*/
    return 0;
}

void MidiChannelPrefix::writeToFile(moduru::io::OutputStream* out, bool writeType)
{
    MetaEvent::writeToFile(out, writeType);
}
