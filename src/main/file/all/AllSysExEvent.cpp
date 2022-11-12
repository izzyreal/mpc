#include "AllSysExEvent.hpp"

#include "AllEvent.hpp"
#include "AllSequence.hpp"

#include <sequencer/MixerEvent.hpp>
#include <sequencer/SystemExclusiveEvent.hpp>

#include <VecUtil.hpp>

#include <cmath>

using namespace mpc::file::all;
using namespace mpc::sequencer;
using namespace moduru;

std::vector<char> AllSysExEvent::MIXER_SIGNATURE = { (char) 240, 71, 0, 68, 69 };

std::shared_ptr<Event> AllSysExEvent::bytesToMpcEvent(const std::vector<char>& bytes)
{
	int byteCount = bytes[BYTE_COUNT_OFFSET];
	
    std::vector<char> sysexLoadData(byteCount);
	
    std::shared_ptr<Event> event;

    for (int i = 0; i < byteCount; i++)
		sysexLoadData[i] = bytes[DATA_OFFSET + i];

	if (VecUtil::Equals(VecUtil::CopyOfRange(sysexLoadData, MIXER_SIGNATURE_OFFSET, MIXER_SIGNATURE_OFFSET + MIXER_SIGNATURE.size()), MIXER_SIGNATURE))
    {
		event = std::make_shared<MixerEvent>();
        auto mixerEvent = std::dynamic_pointer_cast<MixerEvent>(event);
        auto paramCandidate = sysexLoadData[MIXER_PARAMETER_OFFSET] - 1;
		
        if (paramCandidate == 4)
            paramCandidate = 3;
		
        mixerEvent->setParameter(paramCandidate);
        mixerEvent->setPadNumber(sysexLoadData[MIXER_PAD_OFFSET]);
        mixerEvent->setValue(sysexLoadData[MIXER_VALUE_OFFSET]);
        mixerEvent->setTick(AllEvent::readTick(bytes));
	}
	else
    {
        event = std::make_shared<SystemExclusiveEvent>();
		auto sysExEvent = std::dynamic_pointer_cast<SystemExclusiveEvent>(event);
		//sysExEvent->setBytes(sysexLoadData);
        sysExEvent->setTick(AllEvent::readTick(bytes));
	}

    event->setTrack(bytes[AllEvent::TRACK_OFFSET]);

    return event;
}

std::vector<char> AllSysExEvent::mpcEventToBytes(std::shared_ptr<Event> event)
{
    std::vector<char> bytes;
    
    auto mixerEvent = std::dynamic_pointer_cast<MixerEvent>(event);
    auto sysExEvent = std::dynamic_pointer_cast<SystemExclusiveEvent>(event);
    
    if (mixerEvent)
    {
		bytes = std::vector<char>(32);
	
        AllEvent::writeTick(bytes, mixerEvent->getTick());
		bytes[AllEvent::TRACK_OFFSET] = (char)(event->getTrack());
		bytes[CHUNK_HEADER_ID_OFFSET] = HEADER_ID;
		bytes[BYTE_COUNT_OFFSET] = 9;
		bytes[DATA_HEADER_ID_OFFSET] = HEADER_ID;
		for (int i = 0; i < MIXER_SIGNATURE.size(); i++)
			bytes[DATA_OFFSET + i] = MIXER_SIGNATURE[i];

		bytes[DATA_OFFSET + MIXER_PAD_OFFSET] = static_cast<int8_t>(mixerEvent->getPad());
		auto paramCandidate = mixerEvent->getParameter();
        
		if (paramCandidate == 3)
			paramCandidate = 4;

		paramCandidate++;
		bytes[DATA_OFFSET + MIXER_PARAMETER_OFFSET] = (char) (paramCandidate);
		bytes[DATA_OFFSET + MIXER_VALUE_OFFSET] = (char) (mixerEvent->getValue());
		bytes[DATA_OFFSET + MIXER_VALUE_OFFSET + 1] = DATA_TERMINATOR_ID;
		bytes[MIX_TERMINATOR_ID_OFFSET] = CHUNK_TERMINATOR_ID;
	}
	else if (sysExEvent)
	{
		AllEvent::writeTick(bytes, sysExEvent->getTick());
		int dataSize = sysExEvent->getBytes().size();
		int dataSegments = static_cast<int>(dataSize / 8.0);
		bytes = std::vector<char>((dataSegments + 2) * AllSequence::EVENT_SEG_LENGTH);
		bytes[AllEvent::TRACK_OFFSET] = (char) (event->getTrack());
		bytes[AllEvent::TRACK_OFFSET + ( (dataSegments + 1) * AllSequence::EVENT_SEG_LENGTH )] = (char) (event->getTrack());
		bytes[CHUNK_HEADER_ID_OFFSET] = HEADER_ID;
		bytes[BYTE_COUNT_OFFSET] = (char) (dataSize);
		
		for (int i = 0; i < dataSize; i++)
					bytes[DATA_OFFSET + i] = sysExEvent->getBytes()[i];
		
		bytes[(int)(bytes.size()) - 4] = CHUNK_TERMINATOR_ID;
	}
    
    return bytes;
}
