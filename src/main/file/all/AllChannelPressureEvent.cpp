#include <file/all/AllChannelPressureEvent.hpp>

#include <file/all/AllEvent.hpp>
#include <sequencer/ChannelPressureEvent.hpp>
#include <sequencer/Event.hpp>

using namespace mpc::file::all;
using namespace std;

AllChannelPressureEvent::AllChannelPressureEvent(vector<char> ba) 
{
	auto cpe = new mpc::sequencer::ChannelPressureEvent();
	cpe->setTick(AllEvent::readTick(ba));
	cpe->setTrack(ba[AllEvent::TRACK_OFFSET]);
	cpe->setAmount(ba[AMOUNT_OFFSET]);
	event = cpe;
}

AllChannelPressureEvent::AllChannelPressureEvent(mpc::sequencer::Event* e) 
{
	auto cpe = dynamic_cast< mpc::sequencer::ChannelPressureEvent* >(e);
	saveBytes = vector<char>(8);
	saveBytes[AllEvent::EVENT_ID_OFFSET] = AllEvent::CH_PRESSURE_ID;
	AllEvent::writeTick(saveBytes, static_cast< int >(e->getTick()));
	saveBytes[AllEvent::TRACK_OFFSET] = static_cast< int8_t >(e->getTrack());
	saveBytes[AMOUNT_OFFSET] = static_cast< int8_t >(cpe->getAmount());
}

int AllChannelPressureEvent::AMOUNT_OFFSET = 5;
