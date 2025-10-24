#include "AllPolyPressureEvent.hpp"

#include "file/all/AllEvent.hpp"
#include "sequencer/PolyPressureEvent.hpp"

using namespace mpc::file::all;
using namespace mpc::sequencer;

std::shared_ptr<PolyPressureEvent> AllPolyPressureEvent::bytesToMpcEvent(const std::vector<char>& bytes)
{
	auto event = std::make_shared<PolyPressureEvent>();
	
    event->setTick(AllEvent::readTick(bytes));
	event->setTrack(bytes[AllEvent::TRACK_OFFSET]);
	event->setNote(bytes[NOTE_OFFSET]);
	event->setAmount(bytes[AMOUNT_OFFSET]);
	
    return event;
}

std::vector<char> AllPolyPressureEvent::mpcEventToBytes(std::shared_ptr<PolyPressureEvent> event)
{
	std::vector<char> bytes(8);
	
    bytes[AllEvent::EVENT_ID_OFFSET] = AllEvent::POLY_PRESSURE_ID;
	AllEvent::writeTick(bytes, static_cast< int >(event->getTick()));
	bytes[AllEvent::TRACK_OFFSET] = static_cast< int8_t >(event->getTrack());
	bytes[NOTE_OFFSET] = static_cast< int8_t >(event->getNote());
	bytes[AMOUNT_OFFSET] = static_cast< int8_t >(event->getAmount());
    
    return bytes;
}
