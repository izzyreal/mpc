#include <file/all/AllPolyPressureEvent.hpp>

#include <file/all/AllEvent.hpp>
#include <sequencer/Event.hpp>
#include <sequencer/PolyPressureEvent.hpp>

using namespace mpc::file::all;
using namespace std;

AllPolyPressureEvent::AllPolyPressureEvent(const vector<char>& ba) 
{
	auto ppe = new mpc::sequencer::PolyPressureEvent();
	ppe->setTick(AllEvent::readTick(ba));
	ppe->setTrack(ba[AllEvent::TRACK_OFFSET]);
	ppe->setNote(ba[NOTE_OFFSET]);
	ppe->setAmount(ba[AMOUNT_OFFSET]);
	event = ppe;
}

AllPolyPressureEvent::AllPolyPressureEvent(mpc::sequencer::Event* e) 
{
	auto ppe = dynamic_cast< mpc::sequencer::PolyPressureEvent* >(e);
	saveBytes = vector<char>(8);
	saveBytes[AllEvent::EVENT_ID_OFFSET] = AllEvent::POLY_PRESSURE_ID;
	AllEvent::writeTick(saveBytes, static_cast< int >(e->getTick()));
	saveBytes[AllEvent::TRACK_OFFSET] = static_cast< int8_t >(e->getTrack());
	saveBytes[NOTE_OFFSET] = static_cast< int8_t >(ppe->getNote());
	saveBytes[AMOUNT_OFFSET] = static_cast< int8_t >(ppe->getAmount());
}

int AllPolyPressureEvent::NOTE_OFFSET = 5;
int AllPolyPressureEvent::AMOUNT_OFFSET = 6;
