#include <file/all/AllProgramChangeEvent.hpp>

#include <file/all/AllEvent.hpp>
#include <sequencer/Event.hpp>
#include <sequencer/ProgramChangeEvent.hpp>

using namespace mpc::file::all;
using namespace std;

AllProgramChangeEvent::AllProgramChangeEvent(vector<char> ba) 
{
	auto pce = new mpc::sequencer::ProgramChangeEvent();
	pce->setTick(AllEvent::readTick(ba));
	pce->setTrack(ba[AllEvent::TRACK_OFFSET]);
	pce->setProgram(ba[PROGRAM_OFFSET] + 1);
	event = pce;
}

AllProgramChangeEvent::AllProgramChangeEvent(mpc::sequencer::Event* e) 
{
	auto pce = dynamic_cast< mpc::sequencer::ProgramChangeEvent* >(e);
	saveBytes = vector<char>(8);
	saveBytes[AllEvent::EVENT_ID_OFFSET] = AllEvent::PGM_CHANGE_ID;
	AllEvent::writeTick(saveBytes, static_cast< int >(e->getTick()));
	saveBytes[AllEvent::TRACK_OFFSET] = static_cast< int8_t >(e->getTrack());
	saveBytes[PROGRAM_OFFSET] = static_cast< int8_t >((pce->getProgram() - 1));
}

int AllProgramChangeEvent::PROGRAM_OFFSET = 5;
