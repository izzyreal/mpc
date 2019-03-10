#include <file/all/AllControlChangeEvent.hpp>

#include <file/all/AllEvent.hpp>
#include <sequencer/ControlChangeEvent.hpp>
#include <sequencer/Event.hpp>

using namespace mpc::file::all;
using namespace std;

AllControlChangeEvent::AllControlChangeEvent(vector<char> ba) 
{
	auto cce = new mpc::sequencer::ControlChangeEvent();
	cce->setTick(AllEvent::readTick(ba));
	cce->setTrack(ba[AllEvent::TRACK_OFFSET]);
	cce->setController(ba[CONTROLLER_OFFSET]);
	cce->setAmount(ba[AMOUNT_OFFSET]);
	event = cce;
}

AllControlChangeEvent::AllControlChangeEvent(mpc::sequencer::Event* e) 
{
	auto cce = dynamic_cast< mpc::sequencer::ControlChangeEvent* >(e);
	saveBytes = vector<char>(8);
	saveBytes[AllEvent::EVENT_ID_OFFSET] = AllEvent::CONTROL_CHANGE_ID;
	AllEvent::writeTick(saveBytes, static_cast< int >(e->getTick()));
	saveBytes[AllEvent::TRACK_OFFSET] = static_cast< int8_t >(e->getTrack());
	saveBytes[CONTROLLER_OFFSET] = static_cast< int8_t >(cce->getController());
	saveBytes[AMOUNT_OFFSET] = static_cast< int8_t >(cce->getAmount());
}

int AllControlChangeEvent::CONTROLLER_OFFSET = 5;
int AllControlChangeEvent::AMOUNT_OFFSET = 6;
