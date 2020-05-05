#include "TimingCorrectControls.hpp"

#include <lcdgui/LayeredScreen.hpp>
#include <ui/sequencer/window/SequencerWindowGui.hpp>
#include <ui/sampler/SamplerGui.hpp>
#include <sequencer/Sequence.hpp>
#include <sequencer/Track.hpp>
#include <sequencer/Sequencer.hpp>

using namespace mpc::controls::sequencer::window;
using namespace std;

TimingCorrectControls::TimingCorrectControls()
	: AbstractSequencerControls()
{
}

void TimingCorrectControls::function(int i)
{
	super::function(i);
	switch (i) {
	case 4:
	{
		sequencer.lock()->storeActiveSequenceInUndoPlaceHolder();

		auto lTrk = track.lock();
		lTrk->correctTimeRange(swGui->getTime0(), swGui->getTime1(), sequencer.lock()->getTickValues()[swGui->getNoteValue()]);

		vector<int> noteRange(2);

		if (lTrk->getBusNumber() != 0) {
			if (samplerGui->getNote() != 34) {
				noteRange[0] = samplerGui->getNote();
				noteRange[1] = samplerGui->getNote();
			}
			else {
				noteRange[0] = 0;
				noteRange[1] = 127;
			}
		}
		else {
			noteRange[0] = swGui->getMidiNote0();
			noteRange[1] = swGui->getMidiNote1();
		}
		lTrk->swing(lTrk->getEventRange(swGui->getTime0(), swGui->getTime1()), swGui->getNoteValue(), swGui->getSwing(), noteRange);
		lTrk->shiftTiming(lTrk->getEventRange(swGui->getTime0(), swGui->getTime1()), swGui->isShiftTimingLater(), swGui->getAmount(), sequence.lock()->getLastTick());
		ls.lock()->openScreen("sequencer");
		break;
	}
	}
}

void TimingCorrectControls::turnWheel(int i)
{
	init();
	if (param.compare("notevalue") == 0) {
		swGui->setNoteValue(swGui->getNoteValue() + i);
	}
	else if (param.compare("swing") == 0) {
		swGui->setSwing(swGui->getSwing() + i);
	}
	else if (param.compare("shifttiming") == 0) {
		swGui->setShiftTimingLater(i > 0);
	}
	else if (param.compare("amount") == 0) {
		swGui->setAmount(swGui->getAmount() + i);
	}
	checkAllTimesAndNotes(i);
}
