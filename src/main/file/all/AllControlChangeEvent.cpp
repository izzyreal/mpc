#include "file/all/AllControlChangeEvent.hpp"

#include "file/all/AllEvent.hpp"

using namespace mpc::file::all;
using namespace mpc::performance;

Event
AllControlChangeEvent::bytesToMpcEvent(const std::vector<char> &bytes)
{
    Event e;
    e.type = EventType::ControlChange;
    e.tick = AllEvent::readTick(bytes);
    e.trackIndex = TrackIndex(bytes[AllEvent::TRACK_OFFSET]);
    e.controllerNumber = bytes[CONTROLLER_OFFSET];
    e.controllerValue = bytes[AMOUNT_OFFSET];

    return e;
}

std::vector<char> AllControlChangeEvent::mpcEventToBytes(
    const Event &e)
{
    std::vector<char> bytes(8);

    bytes[AllEvent::EVENT_ID_OFFSET] = AllEvent::CONTROL_CHANGE_ID;
    AllEvent::writeTick(bytes, e.tick);
    bytes[AllEvent::TRACK_OFFSET] = e.trackIndex;
    bytes[CONTROLLER_OFFSET] = e.controllerNumber;
    bytes[AMOUNT_OFFSET] = e.controllerValue;

    return bytes;
}
