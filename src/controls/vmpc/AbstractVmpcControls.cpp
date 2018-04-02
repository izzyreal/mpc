#include <controls/vmpc/AbstractVmpcControls.hpp>

#include <Mpc.hpp>
#include <audiomidi/AudioMidiServices.hpp>
//#include <audiomidi/MpcMidiPorts.hpp>
#include <ui/midisync/MidiSyncGui.hpp>
#include <ui/vmpc/AudioGui.hpp>
#include <ui/vmpc/DirectToDiskRecorderGui.hpp>
#include <ui/vmpc/MidiGui.hpp>
#include <sequencer/Sequence.hpp>
#include <sequencer/SeqUtil.hpp>
#include <sequencer/Sequencer.hpp>

using namespace mpc::controls::vmpc;
using namespace std;

AbstractVmpcControls::AbstractVmpcControls(mpc::Mpc* mpc)
	: AbstractControls(mpc)
{
	midiSyncGui = mpc->getUis().lock()->getMidiSyncGui();
	//mpcMidiPorts = mpc->getMidiPorts().lock().get();
	midiGui = mpc->getUis().lock()->getMidiGui();
	audioGui = mpc->getUis().lock()->getAudioGui();
	d2dRecorderGui = mpc->getUis().lock()->getD2DRecorderGui();
	ams = mpc->getAudioMidiServices();
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
		d2dRecorderGui->setTime0(mpc::sequencer::SeqUtil::setClockNumber((mpc::sequencer::SeqUtil::getClockNumber(sequence, d2dRecorderGui->getTime0())) + i, sequence, d2dRecorderGui->getTime0()));
	}
	else if (param.compare("time3") == 0) {
		d2dRecorderGui->setTime1(mpc::sequencer::SeqUtil::getTickFromBar((mpc::sequencer::SeqUtil::getBarFromTick(sequence, d2dRecorderGui->getTime1())) + i, sequence, d2dRecorderGui->getTime1()));
	}
	else if (param.compare("time4") == 0) {
		d2dRecorderGui->setTime1(mpc::sequencer::SeqUtil::setBeat((mpc::sequencer::SeqUtil::getBeat(sequence, d2dRecorderGui->getTime1())) + i, sequence, d2dRecorderGui->getTime1()));
	}
	else if (param.compare("time5") == 0) {
		d2dRecorderGui->setTime1(mpc::sequencer::SeqUtil::setClockNumber((mpc::sequencer::SeqUtil::getClockNumber(sequence, d2dRecorderGui->getTime1())) + i, sequence, d2dRecorderGui->getTime1()));
	}
}

AbstractVmpcControls::~AbstractVmpcControls() {
}
