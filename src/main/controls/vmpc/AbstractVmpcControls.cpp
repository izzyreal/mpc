#include "AbstractVmpcControls.hpp"

#include <Mpc.hpp>
#include <audiomidi/AudioMidiServices.hpp>
#include <ui/midisync/MidiSyncGui.hpp>
#include <ui/vmpc/DirectToDiskRecorderGui.hpp>
#include <sequencer/Sequence.hpp>
#include <sequencer/SeqUtil.hpp>
#include <sequencer/Sequencer.hpp>

using namespace mpc::controls::vmpc;
using namespace std;

AbstractVmpcControls::AbstractVmpcControls()
	: BaseControls()
{
	midiSyncGui = Mpc::instance().getUis().lock()->getMidiSyncGui();
	d2dRecorderGui = Mpc::instance().getUis().lock()->getD2DRecorderGui();
	ams = Mpc::instance().getAudioMidiServices();
}

void AbstractVmpcControls::init()
{
	super::init();
}

void AbstractVmpcControls::checkAllTimes(int i)
{
	init();
	auto sequence = sequencer.lock()->getSequence(d2dRecorderGui->getSq()).lock().get();
	if (param.compare("time0") == 0) {
		d2dRecorderGui->setTime0(mpc::sequencer::SeqUtil::getTickFromBar((mpc::sequencer::SeqUtil::getBarFromTick(sequence, d2dRecorderGui->getTime0())) + i, sequence, d2dRecorderGui->getTime0()));
	}
	else if (param.compare("time1") == 0) {
		d2dRecorderGui->setTime0(mpc::sequencer::SeqUtil::setBeat((mpc::sequencer::SeqUtil::getBeat(sequence, d2dRecorderGui->getTime0())) + i, sequence, d2dRecorderGui->getTime0()));
	}
	else if (param.compare("time2") == 0) {
		d2dRecorderGui->setTime0(mpc::sequencer::SeqUtil::setClock((mpc::sequencer::SeqUtil::getClock(sequence, d2dRecorderGui->getTime0())) + i, sequence, d2dRecorderGui->getTime0()));
	}
	else if (param.compare("time3") == 0) {
		d2dRecorderGui->setTime1(mpc::sequencer::SeqUtil::getTickFromBar((mpc::sequencer::SeqUtil::getBarFromTick(sequence, d2dRecorderGui->getTime1())) + i, sequence, d2dRecorderGui->getTime1()));
	}
	else if (param.compare("time4") == 0) {
		d2dRecorderGui->setTime1(mpc::sequencer::SeqUtil::setBeat((mpc::sequencer::SeqUtil::getBeat(sequence, d2dRecorderGui->getTime1())) + i, sequence, d2dRecorderGui->getTime1()));
	}
	else if (param.compare("time5") == 0) {
		d2dRecorderGui->setTime1(mpc::sequencer::SeqUtil::setClock((mpc::sequencer::SeqUtil::getClock(sequence, d2dRecorderGui->getTime1())) + i, sequence, d2dRecorderGui->getTime1()));
	}
}

AbstractVmpcControls::~AbstractVmpcControls() {
}
