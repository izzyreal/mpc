#include "file/mid/event/meta/MidiChannelPrefix.hpp"

#include "file/mid/event/MidiEvent.hpp"
#include "file/mid/event/meta/GenericMetaEvent.hpp"
#include "file/mid/event/meta/MetaEventData.hpp"
#include "file/mid/event/meta/MetaEvent.hpp"
#include "file/mid/util/VariableLengthInt.hpp"

using namespace mpc::file::mid::event::meta;

MidiChannelPrefix::MidiChannelPrefix(int tick, int delta, int channel)
    : MetaEvent(tick, delta, MetaEvent::MIDI_CHANNEL_PREFIX)
{
    mChannel = channel;
    mLength = mpc::file::mid::util::VariableLengthInt(4);
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

void MidiChannelPrefix::writeToOutputStream(std::ostream &out)
{
    MetaEvent::writeToOutputStream(out);
    out << (char)1;
    out << (char)mChannel;
}

std::shared_ptr<MetaEvent>
MidiChannelPrefix::parseMidiChannelPrefix(int tick, int delta,
                                          MetaEventData *info)
{
    if (info->length.getValue() != 1)
    {
        return std::make_shared<GenericMetaEvent>(tick, delta, info);
    }
    int channel = info->data[0];
    return std::make_shared<MidiChannelPrefix>(tick, delta, channel);
}

int MidiChannelPrefix::compareTo(mpc::file::mid::event::MidiEvent *other)
{
    return 0;
}
