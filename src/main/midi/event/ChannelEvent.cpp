#include <midi/event/ChannelEvent.hpp>

#include <midi/event/ChannelAftertouch.hpp>
#include <midi/event/Controller.hpp>
#include <midi/event/MidiEvent.hpp>
#include <midi/event/NoteAftertouch.hpp>
#include <midi/event/NoteOff.hpp>
#include <midi/event/NoteOn.hpp>
#include <midi/event/PitchBend.hpp>
#include <midi/event/ProgramChange.hpp>

using namespace mpc::midi::event;

ChannelEvent::ChannelEvent(int tick, int type, int channel, int param1, int param2)
: ChannelEvent(tick, 0, type, channel, param1, param2)
{
}

ChannelEvent::ChannelEvent(int tick, int delta, int type, int channel, int param1, int param2)
: MidiEvent(tick, delta)
{
	mType = type & 0x0F;
	mChannel = channel & 0x0F;
	mValue1 = param1 & 0xFF;
	mValue2 = param2 & 0xFF;
}

int ChannelEvent::getType()
{
    return mType;
}

void ChannelEvent::setChannel(int c)
{
	if (c < 0) {
		c = 0;
	}
	else if (c > 15) {
		c = 15;
	}
	mChannel = c;
}

int ChannelEvent::getChannel()
{
    return mChannel;
}

int ChannelEvent::getEventSize()
{
    switch (mType) {
    case PROGRAM_CHANGE:
    case CHANNEL_AFTERTOUCH:
        return 2;
    default:
        return 3;
    }
}

int ChannelEvent::compareTo(MidiEvent* o)
{ 
	return 1; // hack to emulate MPC2000XL MID writing. this makes sure the event order is not changed after they are added
}

bool ChannelEvent::requiresStatusByte(MidiEvent* prevEvent)
{
	if (prevEvent == nullptr) {
		return true;
	}
	if (dynamic_cast<ChannelEvent*>(prevEvent) == nullptr) {
		return true;
	}
	auto ce = dynamic_cast<ChannelEvent*>(prevEvent);
	return !(mType == ce->getType() && mChannel == ce->getChannel());
}

void ChannelEvent::writeToOutputStream(std::ostream& out, bool writeType)
{
	MidiEvent::writeToOutputStream(out, writeType);
	
	if (writeType) {
		auto typeChannel = (mType << 4) + mChannel;
		out << (char) typeChannel;
	}
	
	out << (char) mValue1;

	if (mType != PROGRAM_CHANGE && mType != CHANNEL_AFTERTOUCH) {
		out << (char) mValue2;
	}
}

std::shared_ptr<ChannelEvent> ChannelEvent::parseChannelEvent(int tick, int delta, int type, int channel, std::stringstream& in)
{
    int val1 = in.get();
    int val2 = 0;
	if (type != PROGRAM_CHANGE && type != CHANNEL_AFTERTOUCH) {
		val2 = in.get();
	}
    switch (type) {
    case NOTE_OFF:
        return std::make_shared<NoteOff>(tick, delta, channel, val1, val2);
    case NOTE_ON:
        return std::make_shared<NoteOn>(tick, delta, channel, val1, val2);
    case NOTE_AFTERTOUCH:
        return std::make_shared<NoteAftertouch>(tick, delta, channel, val1, val2);
    case CONTROLLER:
        return std::make_shared<Controller>(tick, delta, channel, val1, val2);
    case PROGRAM_CHANGE:
        return std::make_shared<ProgramChange>(tick, delta, channel, val1);
    case CHANNEL_AFTERTOUCH:
        return std::make_shared<ChannelAftertouch>(tick, delta, channel, val1);
    case PITCH_BEND:
        return std::make_shared<PitchBend>(tick, delta, channel, val1, val2);
    default:
        return std::make_shared<ChannelEvent>(tick, delta, type, channel, val1, val2);
    }
}

const int ChannelEvent::NOTE_OFF;
const int ChannelEvent::NOTE_ON;
const int ChannelEvent::NOTE_AFTERTOUCH;
const int ChannelEvent::CONTROLLER;
const int ChannelEvent::PROGRAM_CHANGE;
const int ChannelEvent::CHANNEL_AFTERTOUCH;
const int ChannelEvent::PITCH_BEND;
