#include "AllPolyPressureEvent.hpp"

#include "file/all/AllEvent.hpp"

using namespace mpc::file::all;
using namespace mpc::sequencer;

EventState AllPolyPressureEvent::bytesToMpcEvent(const std::vector<char> &bytes)
{
    EventState e;
    e.type = EventType::PolyPressure;
    e.tick = AllEvent::readTick(bytes);
    e.trackIndex = TrackIndex(bytes[AllEvent::TRACK_OFFSET]);
    e.noteNumber = NoteNumber(bytes[NOTE_OFFSET]);
    e.amount = bytes[AMOUNT_OFFSET];

    return e;
}

std::vector<char> AllPolyPressureEvent::mpcEventToBytes(const EventState &e)
{
    std::vector<char> bytes(8);

    bytes[AllEvent::EVENT_ID_OFFSET] = AllEvent::POLY_PRESSURE_ID;
    AllEvent::writeTick(bytes, e.tick);
    bytes[AllEvent::TRACK_OFFSET] = e.trackIndex;
    bytes[NOTE_OFFSET] = e.noteNumber;
    bytes[AMOUNT_OFFSET] = e.amount;

    return bytes;
}
