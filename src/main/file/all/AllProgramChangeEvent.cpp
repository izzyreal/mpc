#include "AllProgramChangeEvent.hpp"

#include <file/all/AllEvent.hpp>
#include <sequencer/ProgramChangeEvent.hpp>

using namespace mpc::file::all;
using namespace mpc::sequencer;
using namespace std;

shared_ptr<ProgramChangeEvent> AllProgramChangeEvent::bytesToMpcEvent(const vector<char>& bytes)
{
	auto event = make_shared<ProgramChangeEvent>();
	
    event->setTick(AllEvent::readTick(bytes));
	event->setTrack(bytes[AllEvent::TRACK_OFFSET]);
	event->setProgram(bytes[PROGRAM_OFFSET] + 1);
	
    return event;
}

vector<char> AllProgramChangeEvent::mpcEventToBytes(shared_ptr<ProgramChangeEvent> event)
{
	vector<char> bytes(8);
    
    bytes[AllEvent::EVENT_ID_OFFSET] = AllEvent::PGM_CHANGE_ID;
	AllEvent::writeTick(bytes, static_cast<int>(event->getTick()));
    bytes[AllEvent::TRACK_OFFSET] = static_cast<int8_t>(event->getTrack());
    bytes[PROGRAM_OFFSET] = static_cast<int8_t>(event->getProgram() - 1);
    
    return bytes;
}
