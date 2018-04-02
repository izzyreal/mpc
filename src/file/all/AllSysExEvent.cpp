#include <file/all/AllSysExEvent.hpp>

#include <file/all/AllEvent.hpp>
#include <file/all/AllSequence.hpp>
#include <sequencer/Event.hpp>
#include <sequencer/MixerEvent.hpp>
#include <sequencer/SystemExclusiveEvent.hpp>

#include <VecUtil.hpp>

#include <cmath>

using namespace mpc::file::all;
using namespace std;

AllSysExEvent::AllSysExEvent(vector<char> ba)
{
	int byteCount = ba[BYTE_COUNT_OFFSET];
	sysexLoadData = vector<char>(byteCount);
	for (int i = 0; i < byteCount; i++)
		sysexLoadData[i] = ba[DATA_OFFSET + i];

	if (moduru::VecUtil::Equals(moduru::VecUtil::CopyOfRange(&sysexLoadData, MIXER_SIGNATURE_OFFSET, MIXER_SIGNATURE_OFFSET + MIXER_SIGNATURE.size()), MIXER_SIGNATURE)) {
		auto me = new mpc::sequencer::MixerEvent();
		auto paramCandidate = sysexLoadData[MIXER_PARAMETER_OFFSET] - 1;
		if (paramCandidate == 4) paramCandidate = 3;
		me->setParameter(paramCandidate);
		me->setPadNumber(sysexLoadData[MIXER_PAD_OFFSET]);
		me->setValue(sysexLoadData[MIXER_VALUE_OFFSET]);
		me->setTick(AllEvent::readTick(ba));
		event = me;
	}
	else {
		auto see = new mpc::sequencer::SystemExclusiveEvent();
		//see->setBytes(sysexLoadData);
		see->setTick(AllEvent::readTick(ba));
		event = see;
	}
	event->setTrack(ba[AllEvent::TRACK_OFFSET]);
}

int AllSysExEvent::CHUNK_HEADER_ID_OFFSET = 4;
int AllSysExEvent::BYTE_COUNT_OFFSET = 5;
int AllSysExEvent::DATA_OFFSET = 8;
int AllSysExEvent::MIX_TERMINATOR_ID_OFFSET = 28;
int AllSysExEvent::DATA_HEADER_ID_OFFSET = 8;
char AllSysExEvent::HEADER_ID = 240;
char AllSysExEvent::DATA_TERMINATOR_ID = 247;
char AllSysExEvent::CHUNK_TERMINATOR_ID = 248;
int AllSysExEvent::MIXER_SIGNATURE_OFFSET = 0;
vector<char> AllSysExEvent::MIXER_SIGNATURE = vector<char>{ (char) 240, 71, 0, 68, 69 };
int AllSysExEvent::MIXER_PARAMETER_OFFSET = 5;
int AllSysExEvent::MIXER_PAD_OFFSET = 6;
int AllSysExEvent::MIXER_VALUE_OFFSET = 7;

AllSysExEvent::AllSysExEvent(mpc::sequencer::Event* e) 
{
	if (dynamic_cast< mpc::sequencer::MixerEvent* >(e) != nullptr) {
		auto me = dynamic_cast< mpc::sequencer::MixerEvent* >(e);
		saveBytes = vector<char>(32);
		AllEvent::writeTick(saveBytes, me->getTick());
		saveBytes[AllEvent::TRACK_OFFSET] = (char)(e->getTrack());
		saveBytes[CHUNK_HEADER_ID_OFFSET] = HEADER_ID;
		saveBytes[BYTE_COUNT_OFFSET] = 9;
		saveBytes[DATA_HEADER_ID_OFFSET] = HEADER_ID;
		for (int i = 0; i < MIXER_SIGNATURE.size(); i++)
			saveBytes[DATA_OFFSET + i] = MIXER_SIGNATURE[i];

		saveBytes[DATA_OFFSET + MIXER_PAD_OFFSET] = static_cast< int8_t >(me->getPad());
		auto paramCandidate = me->getParameter();
		if (paramCandidate == 3)
			paramCandidate = 4;

		paramCandidate++;
		saveBytes[DATA_OFFSET + MIXER_PARAMETER_OFFSET] = (char) (paramCandidate);
		saveBytes[DATA_OFFSET + MIXER_VALUE_OFFSET] = (char) (me->getValue());
		saveBytes[DATA_OFFSET + MIXER_VALUE_OFFSET + 1] = DATA_TERMINATOR_ID;
		saveBytes[MIX_TERMINATOR_ID_OFFSET] = CHUNK_TERMINATOR_ID;
	}
	else if (dynamic_cast< mpc::sequencer::SystemExclusiveEvent* >(e) != nullptr) {
		auto see = dynamic_cast< mpc::sequencer::SystemExclusiveEvent* >(e);
		AllEvent::writeTick(saveBytes, see->getTick());
		int dataSize = see->getBytes()->size();
		int dataSegments = static_cast< int >(dataSize / 8.0);
		saveBytes = vector<char>((dataSegments + 2) * Sequence::EVENT_SEG_LENGTH);
		saveBytes[AllEvent::TRACK_OFFSET] = (char) (e->getTrack());
		saveBytes[AllEvent::TRACK_OFFSET + ((dataSegments + 1) * Sequence::EVENT_SEG_LENGTH)] = (char) (e->getTrack());
		saveBytes[CHUNK_HEADER_ID_OFFSET] = HEADER_ID;
		saveBytes[BYTE_COUNT_OFFSET] = (char) (dataSize);
		for (int i = 0; i < dataSize; i++)
			saveBytes[DATA_OFFSET + i] = (*see->getBytes())[i];

		saveBytes[(int)(saveBytes.size()) - 4] = CHUNK_TERMINATOR_ID;
	}
}
