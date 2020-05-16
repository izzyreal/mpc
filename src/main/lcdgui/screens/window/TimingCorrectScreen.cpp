#include "TimingCorrectScreen.hpp"

#include <lcdgui/screens/window/SeqWindowUtil.hpp>
#include <lcdgui/LayeredScreen.hpp>

#include <ui/sequencer/window/SequencerWindowGui.hpp>
#include <ui/sampler/SamplerGui.hpp>

#include <sequencer/Sequence.hpp>
#include <sequencer/Track.hpp>
#include <sequencer/Sequencer.hpp>

using namespace mpc::lcdgui::screens::window;
using namespace std;

TimingCorrectScreen::TimingCorrectScreen(const int& layer)
	: ScreenComponent("timingcorrect", layer)
{
}

void TimingCorrectScreen::function(int i)
{
	BaseControls::function(i);
	switch (i)
	{
	case 4:
	{
		sequencer.lock()->storeActiveSequenceInUndoPlaceHolder();

		auto swGui = mpc.getUis().lock()->getSequencerWindowGui();
		track.lock()->correctTimeRange(swGui->getTime0(), swGui->getTime1(), sequencer.lock()->getTickValues()[swGui->getNoteValue()]);

		vector<int> noteRange(2);

		if (track.lock()->getBusNumber() != 0) {
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
		track.lock()->swing(track.lock()->getEventRange(swGui->getTime0(), swGui->getTime1()), swGui->getNoteValue(), swGui->getSwing(), noteRange);

		auto sequence = sequencer.lock()->getActiveSequence().lock();
		track.lock()->shiftTiming(track.lock()->getEventRange(swGui->getTime0(), swGui->getTime1()), swGui->isShiftTimingLater(), swGui->getAmount(), sequence->getLastTick());
		ls.lock()->openScreen("sequencer");
		break;
	}
	}
}

void TimingCorrectScreen::turnWheel(int i)
{
	init();
	auto swGui = mpc.getUis().lock()->getSequencerWindowGui();
	
	if (param.compare("notevalue") == 0)
	{
		swGui->setNoteValue(swGui->getNoteValue() + i);
	}
	else if (param.compare("swing") == 0)
	{
		swGui->setSwing(swGui->getSwing() + i);
	}
	else if (param.compare("shifttiming") == 0)
	{
		swGui->setShiftTimingLater(i > 0);
	}
	else if (param.compare("amount") == 0)
	{
		swGui->setAmount(swGui->getAmount() + i);
	}
	
	SeqWindowUtil::checkAllTimesAndNotes(i);
}
