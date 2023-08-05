#include "TimingCorrectScreen.hpp"

#include <sequencer/Track.hpp>
#include <sequencer/SeqUtil.hpp>

#include <Util.hpp>

using namespace mpc::lcdgui::screens::window;
using namespace mpc::sequencer;

TimingCorrectScreen::TimingCorrectScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "timing-correct", layerIndex)
{
}

void TimingCorrectScreen::open()
{
	findField("note1")->setAlignment(Alignment::Centered, 18);
	findField("note1")->setLocation(116, 40);

	auto seq = sequencer->getActiveSequence();

	setTime0(0);
	setTime1(seq->getLastTick());

	displayNoteValue();
	displaySwing();
	displayShiftTiming();
	displayAmount();
	displayTime();
	displayNotes();
}

void TimingCorrectScreen::function(int i)
{
	ScreenComponent::function(i);

	switch (i)
	{
	case 4:
	{
		sequencer->storeActiveSequenceInUndoPlaceHolder();

		std::vector<int> noteRange(2);

		if (track->getBus() != 0)
		{
			if (note0 == 34)
			{
                noteRange[0] = 0;
                noteRange[1] = 127;
			}
			else
			{
                noteRange[0] = note0;
                noteRange[1] = note0;
			}
		}
		else
		{
			noteRange[0] = note0;
			noteRange[1] = note1;
		}

        auto eventRange = track->getEventRange(time0, time1);

		auto sequence = sequencer->getActiveSequence();

        for (auto& e: eventRange)
        {
            if (auto noteEvent = std::dynamic_pointer_cast<NoteOnEvent>(e))
            {
                if (noteEvent->getNote() >= noteRange[0] && noteEvent->getNote() <= noteRange[1])
                {
                    track->shiftTiming(e, shiftTimingLater, amount, sequence->getLastTick());
                }
            }
        }

        track->correctTimeRange(time0, time1, getNoteValueLengthInTicks(), swing, noteRange[0], noteRange[1]);

        openScreen("sequencer");
		break;
	}
	}
}

void TimingCorrectScreen::turnWheel(int i)
{
	init();
	
	if (param == "notevalue")
	{
		setNoteValue(noteValue + i);
	}
	else if (param == "swing")
	{
		setSwing(swing + i);
	}
	else if (param == "shifttiming")
	{
		setShiftTimingLater(i > 0);
	}
	else if (param == "amount")
	{
		setAmount(amount + i);
	}
	
	checkAllTimesAndNotes(mpc, i);
	displayTime();
}

void TimingCorrectScreen::setNote0(int i)
{
    init();

    if (param == "note0")
    {
        WithTimesAndNotes::setNote0(i);
    }
}

void TimingCorrectScreen::displayNoteValue()
{
	findChild<FunctionKey>("fk4")->Hide(noteValue == 0);
	SetDirty();

	findField("notevalue")->setText(noteValueNames[noteValue]);
	findLabel("swing")->Hide(!(noteValue == 1 || noteValue == 3));
	findField("swing")->Hide(!(noteValue == 1 || noteValue == 3));
}

void TimingCorrectScreen::displaySwing()
{
	findField("swing")->setText(std::to_string(swing));
}

void TimingCorrectScreen::displayNotes()
{
	init();
	
	if (track->getBus() == 0)
	{
		findField("note0")->setAlignment(Alignment::Centered, 18);
		findField("note0")->setLocation(62, 40);
		findField("note0")->setSize(47, 9);
		findField("note0")->setText(StrUtil::padLeft(std::to_string(note0), " ", 3) + "(" + mpc::Util::noteNames()[note0] + u8"\u00D4");
		findField("note1")->setText(StrUtil::padLeft(std::to_string(note1), " ", 3) + "(" + mpc::Util::noteNames()[note1] + u8"\u00D4");
		findLabel("note1")->Hide(false);
		findField("note1")->Hide(false);
	}
	else
	{
		findField("note0")->setAlignment(Alignment::None);
		findField("note0")->setLocation(61, 40);
		findField("note0")->setSize(37, 9);
		
		if (note0 == 34)
        {
            findField("note0")->setText("ALL");
        }
		else
        {
            auto padIndex = program->getPadIndexFromNote(note0);
            auto padName = sampler->getPadName(padIndex);
            findField("note0")->setText(std::to_string(note0) + "/" + padName);
        }

		findLabel("note1")->Hide(true);
		findField("note1")->Hide(true);
	}
}

void TimingCorrectScreen::displayShiftTiming()
{
	findField("shifttiming")->setText(shiftTimingLater ? "LATER" : "EARLIER");
}

void TimingCorrectScreen::displayAmount()
{
	findField("amount")->setTextPadded(amount);
}

void TimingCorrectScreen::displayTime()
{
	auto s = sequencer->getActiveSequence().get();
	findField("time0")->setTextPadded(SeqUtil::getBarFromTick(s, time0) + 1, "0");
	findField("time1")->setTextPadded(SeqUtil::getBeat(s, time0) + 1, "0");
	findField("time2")->setTextPadded(SeqUtil::getClock(s, time0), "0");
	findField("time3")->setTextPadded(SeqUtil::getBarFromTick(s, time1) + 1, "0");
	findField("time4")->setTextPadded(SeqUtil::getBeat(s, time1) + 1, "0");
	findField("time5")->setTextPadded(SeqUtil::getClock(s, time1), "0");
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

unsigned char TimingCorrectScreen::getNoteValueLengthInTicks()
{
    return noteValueLengthsInTicks[noteValue];
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

int TimingCorrectScreen::getAmount()
{
    return amount;
}

int TimingCorrectScreen::isShiftTimingLater()
{
    return shiftTimingLater;
}
