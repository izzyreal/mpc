#include "file/all/AllChannelPressureEvent.hpp"

#include "file/all/AllEvent.hpp"
#include "sequencer/ChannelPressureEvent.hpp"

using namespace mpc::file::all;
using namespace mpc::sequencer;

std::shared_ptr<ChannelPressureEvent>
AllChannelPressureEvent::bytesToMpcEvent(const std::vector<char> &bytes)
{
    auto event = std::make_shared<ChannelPressureEvent>();

    event->setTick(AllEvent::readTick(bytes));
    event->setTrack(bytes[AllEvent::TRACK_OFFSET]);
    event->setAmount(bytes[AMOUNT_OFFSET]);

    return event;
}

std::vector<char> AllChannelPressureEvent::mpcEventToBytes(
    const std::shared_ptr<ChannelPressureEvent> &event)
{
    std::vector<char> bytes(8);

    bytes[AllEvent::EVENT_ID_OFFSET] = AllEvent::CH_PRESSURE_ID;
    AllEvent::writeTick(bytes, event->getTick());
    bytes[AllEvent::TRACK_OFFSET] = static_cast<int8_t>(event->getTrack());
    bytes[AMOUNT_OFFSET] = static_cast<int8_t>(event->getAmount());

    return bytes;
}
