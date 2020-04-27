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

void MidiChannelPrefix::writeToOutputStream(ostream& out) 
{
    MetaEvent::writeToOutputStream(out);
    out << 1;
    out << mChannel;
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
    return 0;
}
