#include <lcdgui/screens/window/SeqWindowUtil.hpp>

#include <Mpc.hpp>

#include <lcdgui/LayeredScreen.hpp>

#include <ui/sequencer/window/SequencerWindowGui.hpp>
#include <ui/sampler/SamplerGui.hpp>

#include <sequencer/Sequencer.hpp>
#include <sequencer/Sequence.hpp>
#include <sequencer/Track.hpp>
#include <sequencer/SeqUtil.hpp>

#include <sampler/Sampler.hpp>
#include <sampler/Program.hpp>

#include <mpc/MpcSoundPlayerChannel.hpp>

using namespace mpc::sequencer;
using namespace mpc::lcdgui::screens::window;

using namespace std;

void SeqWindowUtil::checkAllTimesAndNotes(int i)
{
	auto& mpc = mpc::Mpc::instance();
	
	auto sequence = mpc.getSequencer().lock()->getActiveSequence().lock();
	auto param = mpc.getLayeredScreen().lock()->getFocus();
	auto gui = mpc.getUis().lock()->getSequencerWindowGui();

	if (param.compare("time0") == 0) {
		gui->setTime0(SeqUtil::getTickFromBar((SeqUtil::getBarFromTick(sequence.get(), gui->getTime0())) + i, sequence.get(), gui->getTime0()));
	}
	else if (param.compare("time1") == 0) {
		gui->setTime0(SeqUtil::setBeat((SeqUtil::getBeat(sequence.get(), gui->getTime0())) + i, sequence.get(), gui->getTime0()));
	}
	else if (param.compare("time2") == 0) {
		gui->setTime0(SeqUtil::setClockNumber((SeqUtil::getClockNumber(sequence.get(), gui->getTime0())) + i, sequence.get(), gui->getTime0()));
	}
	else if (param.compare("time3") == 0) {
		gui->setTime1(SeqUtil::getTickFromBar((SeqUtil::getBarFromTick(sequence.get(), gui->getTime1())) + i, sequence.get(), gui->getTime1()));
	}
	else if (param.compare("time4") == 0) {
		gui->setTime1(SeqUtil::setBeat((SeqUtil::getBeat(sequence.get(), gui->getTime1())) + i, sequence.get(), gui->getTime1()));
	}
	else if (param.compare("time5") == 0) {
		gui->setTime1(SeqUtil::setClockNumber((SeqUtil::getClockNumber(sequence.get(), gui->getTime1())) + i, sequence.get(), gui->getTime1()));
	}
	else if (param.compare("notes0") == 0) {

		auto track = mpc.getSequencer().lock()->getActiveTrack().lock();

		if (track->getBusNumber() != 0) {
			
			auto samplerGui = mpc::Mpc::instance().getUis().lock()->getSamplerGui();
			auto note = samplerGui->getNote() + i;
			auto mpcSoundPlayerChannel = mpc.getSampler().lock()->getDrum(track->getBusNumber() - 1);
			auto program = dynamic_pointer_cast<mpc::sampler::Program>(mpc.getSampler().lock()->getProgram(mpcSoundPlayerChannel->getProgram()).lock());
			auto pad = program->getPadNumberFromNote(note);
			samplerGui->setPadAndNote(pad, samplerGui->getNote());
		}
		else {
			gui->setMidiNote0(gui->getMidiNote0() + i);
		}
	}
	else if (param.compare("notes1") == 0) {
		gui->setMidiNote1(gui->getMidiNote1() + i);
	}
}
