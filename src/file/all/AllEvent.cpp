#include <file/all/AllEvent.hpp>

#include <Util.hpp>
#include <file/BitUtil.hpp>
//#include <file/Definitions.hpp>
#include <file/all/AllChannelPressureEvent.hpp>
#include <file/all/AllControlChangeEvent.hpp>
#include <file/all/AllNoteEvent.hpp>
#include <file/all/AllPitchBendEvent.hpp>
#include <file/all/AllPolyPressureEvent.hpp>
#include <file/all/AllProgramChangeEvent.hpp>
#include <file/all/AllSysExEvent.hpp>
#include <sequencer/ChannelPressureEvent.hpp>
#include <sequencer/ControlChangeEvent.hpp>
#include <sequencer/Event.hpp>
#include <sequencer/MixerEvent.hpp>
#include <sequencer/NoteEvent.hpp>
#include <sequencer/PitchBendEvent.hpp>
#include <sequencer/PolyPressureEvent.hpp>
#include <sequencer/ProgramChangeEvent.hpp>
#include <sequencer/SystemExclusiveEvent.hpp>

#include <cmath>

#include <file/ByteUtil.hpp>

using namespace mpc::file::all;
using namespace std;

AllEvent::AllEvent(vector<char> ba) 
{
	auto eventID = ba[EVENT_ID_OFFSET];
	if (eventID < 0) {
		switch (ba[EVENT_ID_OFFSET]) {
		case POLY_PRESSURE_ID:
			event = (new AllPolyPressureEvent(ba))->event;
			break;
		case CONTROL_CHANGE_ID:
			event = (new AllControlChangeEvent(ba))->event;
			break;
		case PGM_CHANGE_ID:
			event = (new AllProgramChangeEvent(ba))->event;
			break;
		case CH_PRESSURE_ID:
			event = (new AllChannelPressureEvent(ba))->event;
			break;
		case PITCH_BEND_ID:
			event = (new AllPitchBendEvent(ba))->event;
			break;
		case SYS_EX_ID:
			event = (new AllSysExEvent(ba))->event;
			break;
		}

	}
	else {
		auto ane = new AllNoteEvent(ba);
		auto ne = new mpc::sequencer::NoteEvent(ane->getNote());
		ne->setDuration(ane->getDuration());
		ne->setTick(ane->getTick());
		ne->setVelocity(ane->getVelocity());
		ne->setVariationValue(ane->getVariationValue());
		ne->setVariationTypeNumber(ane->getVariationType());
		ne->setTrack(ba[TRACK_OFFSET]);
		event = ne;
	}
}

AllEvent::AllEvent(mpc::sequencer::Event* event)
{
	if (dynamic_cast<mpc::sequencer::NoteEvent*>(event) != nullptr) {
		bytes = AllNoteEvent(event).getBytes();
	}
	else if (dynamic_cast<mpc::sequencer::PolyPressureEvent*>(event) != nullptr) {
		bytes = AllPolyPressureEvent(event).saveBytes;
	}
	else if (dynamic_cast<mpc::sequencer::ControlChangeEvent*>(event) != nullptr) {
		bytes = AllControlChangeEvent(event).saveBytes;
	}
	else if (dynamic_cast<mpc::sequencer::ProgramChangeEvent*>(event) != nullptr) {
		bytes = AllProgramChangeEvent(event).saveBytes;
	}
	else if (dynamic_cast<mpc::sequencer::ChannelPressureEvent*>(event) != nullptr) {
		bytes = AllChannelPressureEvent(event).saveBytes;
	}
	else if (dynamic_cast<mpc::sequencer::PitchBendEvent*>(event) != nullptr) {
		bytes = AllPitchBendEvent(event).saveBytes;
	}
	else if (dynamic_cast<mpc::sequencer::SystemExclusiveEvent*>(event) != nullptr || dynamic_cast<mpc::sequencer::MixerEvent*>(event) != nullptr) {
		bytes = AllSysExEvent(event).saveBytes;
	}
}

const int AllEvent::TICK_BYTE1_OFFSET;
const int AllEvent::TICK_BYTE2_OFFSET;
const int AllEvent::TICK_BYTE3_OFFSET;

vector<int> AllEvent::TICK_BYTE3_BIT_RANGE = vector<int>{ 0, 3 };

const int AllEvent::TRACK_OFFSET;
const int AllEvent::EVENT_ID_OFFSET;

const char AllEvent::POLY_PRESSURE_ID;
const char AllEvent::CONTROL_CHANGE_ID;
const char AllEvent::PGM_CHANGE_ID;
const char AllEvent::CH_PRESSURE_ID;
const char AllEvent::PITCH_BEND_ID;
const char AllEvent::SYS_EX_ID;

mpc::sequencer::Event* AllEvent::getEvent()
{
    return event;
}

vector<char> AllEvent::getBytes()
{
    return bytes;
}

int AllEvent::readTick(vector<char> b)
{
	unsigned short s3 = moduru::file::BitUtil::removeUnusedBits(b[TICK_BYTE3_OFFSET], TICK_BYTE3_BIT_RANGE);
	int result = moduru::file::ByteUtil::bytes2ushort(vector<char>{ b[TICK_BYTE1_OFFSET], b[TICK_BYTE2_OFFSET] }) + (s3 * 65536);
	return result;
}

vector<char> AllEvent::writeTick(vector<char> event, int tick)
{
	auto remainder = tick % 65536;
	auto ba = moduru::file::ByteUtil::ushort2bytes(remainder);
	event[TICK_BYTE1_OFFSET] = ba[0];
	event[TICK_BYTE2_OFFSET] = ba[1];
	auto s3 = static_cast<int16_t>(floor(tick / 65536.0));
	event[TICK_BYTE3_OFFSET] = moduru::file::BitUtil::stitchBytes(event[TICK_BYTE3_OFFSET], AllNoteEvent::DURATION_BYTE1_BIT_RANGE, static_cast<int8_t>(s3), TICK_BYTE3_BIT_RANGE);
	return event;
}
