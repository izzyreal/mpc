#include <file/all/AllPitchBendEvent.hpp>

#include <Util.hpp>
#include <file/all/AllEvent.hpp>
#include <sequencer/Event.hpp>
#include <sequencer/PitchBendEvent.hpp>

#include <file/ByteUtil.hpp>

using namespace mpc::file::all;
using namespace std;

AllPitchBendEvent::AllPitchBendEvent(const vector<char>& ba)
{
	auto pbe = new mpc::sequencer::PitchBendEvent();
	pbe->setTick(AllEvent::readTick(ba));
	pbe->setTrack(ba[AllEvent::TRACK_OFFSET]);

    auto candidate = moduru::file::ByteUtil::bytes2ushort(vector<char>{ ba[AMOUNT_OFFSET], ba[AMOUNT_OFFSET + 1] }) - 16384;

    if (candidate < -8192)
		candidate += 8192;

	pbe->setAmount(candidate);
	event = pbe;
}

AllPitchBendEvent::AllPitchBendEvent(mpc::sequencer::Event* e) 
{
	auto pbe = dynamic_cast< mpc::sequencer::PitchBendEvent* >(e);
	saveBytes = vector<char>(8);
	saveBytes[AllEvent::EVENT_ID_OFFSET] = AllEvent::PITCH_BEND_ID;
	AllEvent::writeTick(saveBytes, static_cast< int >(e->getTick()));
	saveBytes[AllEvent::TRACK_OFFSET] = static_cast< int8_t >(e->getTrack());
	auto candidate = pbe->getAmount() + 16384;
	if (pbe->getAmount() < 0)
		candidate = pbe->getAmount() + 8192;

	auto amountBytes = moduru::file::ByteUtil::ushort2bytes(candidate);
	saveBytes[AMOUNT_OFFSET] = amountBytes[0];
	saveBytes[AMOUNT_OFFSET + 1] = amountBytes[1];
}

int AllPitchBendEvent::AMOUNT_OFFSET = 5;
