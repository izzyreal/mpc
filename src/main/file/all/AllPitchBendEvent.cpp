#include "file/all/AllPitchBendEvent.hpp"

#include "file/all/AllEvent.hpp"
#include "sequencer/PitchBendEvent.hpp"

#include "file/ByteUtil.hpp"

using namespace mpc::file::all;
using namespace mpc::sequencer;

std::shared_ptr<PitchBendEvent> AllPitchBendEvent::bytesToMpcEvent(const std::vector<char>& bytes)
{
	auto event = std::make_shared<PitchBendEvent>();
	event->setTick(AllEvent::readTick(bytes));
	event->setTrack(bytes[AllEvent::TRACK_OFFSET]);

    auto candidate = ByteUtil::bytes2ushort(std::vector<char>{ bytes[AMOUNT_OFFSET], bytes[AMOUNT_OFFSET + 1] }) - 16384;

    if (candidate < -8192)
		candidate += 8192;

	event->setAmount(candidate);
    
    return event;
}

std::vector<char> AllPitchBendEvent::mpcEventToBytes(std::shared_ptr<PitchBendEvent> event)
{
	std::vector<char> bytes(8);
	bytes[AllEvent::EVENT_ID_OFFSET] = AllEvent::PITCH_BEND_ID;
	AllEvent::writeTick(bytes, static_cast< int >(event->getTick()));
	bytes[AllEvent::TRACK_OFFSET] = static_cast< int8_t >(event->getTrack());
	
    auto candidate = event->getAmount() + 16384;
	
    if (event->getAmount() < 0)
		candidate = event->getAmount() + 8192;

	auto amountBytes = ByteUtil::ushort2bytes(candidate);
	
    bytes[AMOUNT_OFFSET] = amountBytes[0];
	bytes[AMOUNT_OFFSET + 1] = amountBytes[1];
    
    return bytes;
}
