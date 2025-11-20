#include "AllProgramChangeEvent.hpp"

#include "file/all/AllEvent.hpp"

using namespace mpc::file::all;
using namespace mpc::performance;

Event
AllProgramChangeEvent::bytesToMpcEvent(const std::vector<char> &bytes)
{
    Event e;
    e.type = EventType::ProgramChange;
    e.tick = AllEvent::readTick(bytes);
    e.trackIndex = TrackIndex(bytes[AllEvent::TRACK_OFFSET]);
    e.programChangeProgramIndex = ProgramIndex(bytes[PROGRAM_OFFSET] + 1);

    return e;
}

std::vector<char> AllProgramChangeEvent::mpcEventToBytes(
    const Event &e)
{
    std::vector<char> bytes(8);

    bytes[AllEvent::EVENT_ID_OFFSET] = AllEvent::PGM_CHANGE_ID;
    AllEvent::writeTick(bytes, e.tick);
    bytes[AllEvent::TRACK_OFFSET] = e.trackIndex;
    bytes[PROGRAM_OFFSET] = e.programChangeProgramIndex;

    return bytes;
}
