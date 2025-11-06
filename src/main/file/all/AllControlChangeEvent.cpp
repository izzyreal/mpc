#include "file/all/AllControlChangeEvent.hpp"

#include "file/all/AllEvent.hpp"
#include "sequencer/ControlChangeEvent.hpp"

using namespace mpc::file::all;
using namespace mpc::sequencer;

std::shared_ptr<ControlChangeEvent>
AllControlChangeEvent::bytesToMpcEvent(const std::vector<char> &bytes)
{
    auto event = std::make_shared<ControlChangeEvent>();

    event->setTick(AllEvent::readTick(bytes));
    event->setTrack(bytes[AllEvent::TRACK_OFFSET]);
    event->setController(bytes[CONTROLLER_OFFSET]);
    event->setAmount(bytes[AMOUNT_OFFSET]);

    return event;
}

std::vector<char> AllControlChangeEvent::mpcEventToBytes(
    const std::shared_ptr<ControlChangeEvent> &event)
{
    std::vector<char> bytes(8);

    bytes[AllEvent::EVENT_ID_OFFSET] = AllEvent::CONTROL_CHANGE_ID;
    AllEvent::writeTick(bytes, static_cast<int>(event->getTick()));
    bytes[AllEvent::TRACK_OFFSET] = static_cast<int8_t>(event->getTrack());
    bytes[CONTROLLER_OFFSET] = static_cast<int8_t>(event->getController());
    bytes[AMOUNT_OFFSET] = static_cast<int8_t>(event->getAmount());

    return bytes;
}
