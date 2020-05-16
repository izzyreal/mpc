#include "TimingCorrectScreen.hpp"

#include <lcdgui/screens/window/SeqWindowUtil.hpp>
#include <lcdgui/LayeredScreen.hpp>
#include <lcdgui/FunctionKeys.hpp>
#include <lcdgui/Label.hpp>

#include <ui/sampler/SamplerGui.hpp>

#include <sequencer/Sequence.hpp>
#include <sequencer/Track.hpp>
#include <sequencer/Sequencer.hpp>
#include <sequencer/SeqUtil.hpp>

#include <lang/StrUtil.hpp>

using namespace mpc::lcdgui::screens::window;
using namespace mpc::sequencer;
using namespace std;

TimingCorrectScreen::TimingCorrectScreen(const int& layer)
	: ScreenComponent("timingcorrect", layer)
{
}

void TimingCorrectScreen::open()
{
	auto seq = sequencer.lock()->getActiveSequence();
	//setTime1(seq->getLastTick());
	displayNoteValue();
	displaySwing();
	displayShiftTiming();
	displayAmount();
	displayTime();
	displayNotes();
}

void TimingCorrectScreen::function(int i)
{
	BaseControls::function(i);
	switch (i)
	{
	case 4:
	{
		sequencer.lock()->storeActiveSequenceInUndoPlaceHolder();

		track.lock()->correctTimeRange(time0, time1, sequencer.lock()->getTickValues()[noteValue]);

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
			noteRange[0] = midiNote0;
			noteRange[1] = midiNote1;
		}
		track.lock()->swing(track.lock()->getEventRange(time0, time1), noteValue, swing, noteRange);

		auto sequence = sequencer.lock()->getActiveSequence().lock();
		track.lock()->shiftTiming(track.lock()->getEventRange(time0, time1), shiftTimingLater, amount, sequence->getLastTick());
		ls.lock()->openScreen("sequencer");
		break;
	}
	}
}

void TimingCorrectScreen::turnWheel(int i)
{
	init();
	
	if (param.compare("notevalue") == 0)
	{
		setNoteValue(noteValue + i);
	}
	else if (param.compare("swing") == 0)
	{
		setSwing(swing + i);
	}
	else if (param.compare("shifttiming") == 0)
	{
		setShiftTimingLater(i > 0);
	}
	else if (param.compare("amount") == 0)
	{
		setAmount(amount + i);
	}
	
	SeqWindowUtil::checkAllTimesAndNotes(i);
}

void TimingCorrectScreen::displayNoteValue()
{
	auto fb = dynamic_pointer_cast<mpc::lcdgui::FunctionKeys>(findChild("function-keys").lock());

	if (noteValue != 0)
	{
		fb->enable(4);
	}
	else {
		fb->disable(4);
	}

	findField("notevalue").lock()->setText(noteValueNames[noteValue]);
	findLabel("swing").lock()->Hide(!(noteValue == 1 || noteValue == 3));
	findField("swing").lock()->Hide(!(noteValue == 1 || noteValue == 3));
}

void TimingCorrectScreen::displaySwing()
{
	findField("swing").lock()->setText(to_string(swing));
}

void TimingCorrectScreen::displayNotes()
{
	init();
	
	if (track.lock()->getBusNumber() == 0) {
		findField("notes0").lock()->setSize(8 * 6, 9);
		findField("notes0").lock()->setText(moduru::lang::StrUtil::padLeft(to_string(midiNote0), " ", 3) + "(" + mpc::ui::Uis::noteNames[midiNote0] + u8"\u00D4");
		findField("notes1").lock()->setText(moduru::lang::StrUtil::padLeft(to_string(midiNote1), " ", 3) + "(" + mpc::ui::Uis::noteNames[midiNote1] + u8"\u00D4");
		findLabel("notes1").lock()->Hide(false);
		findField("notes1").lock()->Hide(false);
	}
	else {
		findField("notes0").lock()->setSize(6 * 6 + 2, 9);
		
		if (samplerGui->getNote() != 34) {
			findField("notes0").lock()->setText(to_string(samplerGui->getNote()) + "/" + sampler.lock()->getPadName(samplerGui->getPad()));
		}
		else {
			findField("notes0").lock()->setText("ALL");
		}
		findLabel("notes1").lock()->Hide(true);
		findField("notes1").lock()->Hide(true);
	}
}

void TimingCorrectScreen::displayShiftTiming()
{
	findField("shifttiming").lock()->setText(shiftTimingLater ? "LATER" : "EARLIER");
}

void TimingCorrectScreen::displayAmount()
{
	findField("amount").lock()->setText(to_string(amount));
}

void TimingCorrectScreen::displayTime()
{
	auto s = sequencer.lock()->getActiveSequence().lock().get();
	findField("time0").lock()->setTextPadded(SeqUtil::getBarFromTick(s, time0) + 1, "0");
	findField("time1").lock()->setTextPadded(SeqUtil::getBeat(s, time0) + 1, "0");
	findField("time2").lock()->setTextPadded(SeqUtil::getClockNumber(s, time0), "0");
	findField("time3").lock()->setTextPadded(SeqUtil::getBarFromTick(s, time1) + 1, "0");
	findField("time4").lock()->setTextPadded(SeqUtil::getBeat(s, time1) + 1, "0");
	findField("time5").lock()->setTextPadded(SeqUtil::getClockNumber(s, time1), "0");
}

void TimingCorrectScreen::setAmount(int i)
{
	if (i < 0)
	{
		return;
	}

	if (noteValue == 0 && i > 0)
		return;

	if (noteValue == 1 && i > 23)
		return;

	if (noteValue == 2 && i > 15)
		return;

	if (noteValue == 3 && i > 11)
		return;

	if (noteValue == 4 && i > 7)
		return;

	if (noteValue == 5 && i > 5)
		return;

	if (noteValue == 6 && i > 3)
		return;

	amount = i;
	displayAmount();
}

void TimingCorrectScreen::setShiftTimingLater(bool b)
{
	shiftTimingLater = b;
	displayShiftTiming();
}

void TimingCorrectScreen::setMidiNote0(int i)
{
	if (i < 0 || i > 127)
	{
		return;
	}

	midiNote0 = i;
	displayNotes();
}

void TimingCorrectScreen::setMidiNote1(int i)
{
	if (i < 0 || i > 127)
	{
		return;
	}

	midiNote1 = i;
	displayNotes();
}

int TimingCorrectScreen::getSwing()
{
	return swing;
}

void TimingCorrectScreen::setSwing(int i)
{
	if (i < 50 || i > 75)
	{
		return;
	}

	swing = i;
	displaySwing();
}

int TimingCorrectScreen::getNoteValue()
{
	return noteValue;
}

void TimingCorrectScreen::setNoteValue(int i)
{
	if (i < 0 || i > 6)
	{
		return;
	}

	noteValue = i;
	
	if (noteValue == 0)
	{
		setAmount(0);
	}
	else if (noteValue == 2)
	{
		if (amount > 15)
		{
			setAmount(15);
		}
	}
	else if (noteValue == 3)
	{
		if (amount > 11)
		{
			setAmount(11);
		}
	}
	else if (noteValue == 4)
	{
		if (amount > 7)
		{
			setAmount(7);
		}
	}
	else if (noteValue == 5)
	{
		if (amount > 5)
		{
			setAmount(5);
		}
	}
	else if (noteValue == 6)
	{
		if (amount > 3)
		{
			setAmount(3);
		}
	}
	
	init();
	displayNoteValue();
}

void TimingCorrectScreen::setTime0(int time0)
{
	this->time0 = time0;

	if (time0 > time1)
	{
		time1 = time0;
	}

	displayTime();
}

void TimingCorrectScreen::setTime1(int time1)
{
	this->time1 = time1;

	if (time1 < time0)
	{
		time0 = time1;
	}
	displayTime();
}
