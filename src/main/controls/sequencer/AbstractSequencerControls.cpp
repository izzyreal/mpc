#include "AbstractSequencerControls.hpp"

#include <Mpc.hpp>

#include <ui/Uis.hpp>
#include <ui/sampler/SamplerGui.hpp>
#include <ui/sequencer/BarCopyGui.hpp>
#include <ui/sequencer/EditSequenceGui.hpp>
#include <ui/sequencer/SongGui.hpp>
#include <ui/sequencer/StepEditorGui.hpp>
#include <ui/sequencer/TrMoveGui.hpp>
#include <ui/sequencer/window/SequencerWindowGui.hpp>
#include <sampler/Program.hpp>
#include <sequencer/Sequence.hpp>
#include <sequencer/Track.hpp>
#include <sequencer/SeqUtil.hpp>
#include <sequencer/Sequencer.hpp>

using namespace mpc::controls::sequencer;
using namespace mpc::sequencer;
using namespace std;

AbstractSequencerControls::AbstractSequencerControls()
	: BaseControls()
{
	swGui = Mpc::instance().getUis().lock()->getSequencerWindowGui();
	seGui = Mpc::instance().getUis().lock()->getStepEditorGui();
	songGui = Mpc::instance().getUis().lock()->getSongGui();
	barCopyGui = Mpc::instance().getUis().lock()->getBarCopyGui();
	editSequenceGui = Mpc::instance().getUis().lock()->getEditSequenceGui();
	trMoveGui = Mpc::instance().getUis().lock()->getTrMoveGui();
}

void AbstractSequencerControls::init()
{
	BaseControls::init();
	
	if (csn.compare("barcopy") == 0) {
		fromSeq = sequencer.lock()->getSequence(barCopyGui->getFromSq());
		toSeq = sequencer.lock()->getSequence(barCopyGui->getToSq());
	}
	else {
		fromSeq = sequencer.lock()->getSequence(editSequenceGui->getFromSq());
		toSeq = sequencer.lock()->getSequence(editSequenceGui->getToSq());
	}
	sequence = sequencer.lock()->getActiveSequence();
}
