#include "file/all/AllChannelPressureEvent.hpp"

#include "file/all/AllEvent.hpp"

using namespace mpc::file::all;
using namespace mpc::sequencer;

EventData
AllChannelPressureEvent::bytesToMpcEvent(const std::vector<char> &bytes)
{
    EventData e;
    e.type = EventType::ChannelPressure;
    e.tick = AllEvent::readTick(bytes);
    e.trackIndex = TrackIndex(bytes[AllEvent::TRACK_OFFSET]);
    e.amount = bytes[AMOUNT_OFFSET];
    return e;
}

std::vector<char> AllChannelPressureEvent::mpcEventToBytes(const EventData &e)
{
    std::vector<char> bytes(8);

    bytes[AllEvent::EVENT_ID_OFFSET] = AllEvent::CH_PRESSURE_ID;
    AllEvent::writeTick(bytes, e.tick);
    bytes[AllEvent::TRACK_OFFSET] = e.trackIndex;
    bytes[AMOUNT_OFFSET] = e.amount;

    return bytes;
}
