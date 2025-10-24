#include "EditVelocityScreen.hpp"

#include "sequencer/Track.hpp"
#include "sequencer/NoteEvent.hpp"
#include "sequencer/SeqUtil.hpp"

#include <Util.hpp>

#include <StrUtil.hpp>

using namespace mpc::lcdgui::screens::window;
using namespace mpc::sequencer;


EditVelocityScreen::EditVelocityScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "edit-velocity", layerIndex)
{
}

void EditVelocityScreen::setNote0(int i)
{

    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName == "note0")
    {
        WithTimesAndNotes::setNote0(i);
    }
}

void EditVelocityScreen::open()
{
	auto bus = sequencer.lock()->getActiveTrack()->getBus();

	if (bus == 0)
	{
		findField("note0")->setAlignment(Alignment::Centered, 18);
		findField("note1")->setAlignment(Alignment::Centered, 18);
		findField("note0")->setLocation(62, 42);
	}
	else
	{
		findField("note0")->setAlignment(Alignment::None);
		findField("note1")->setAlignment(Alignment::None);
		findField("note0")->setLocation(61, 42);
	}

	findField("note1")->setLocation(116, 42);

	auto seq = sequencer.lock()->getActiveSequence();

	setTime0(0);
	setTime1(seq->getLastTick());
	
	displayEditType();
	displayValue();
	displayTime();
	displayNotes();
}

void EditVelocityScreen::function(int i)
{
	ScreenComponent::function(i);

    auto track = mpc.getSequencer()->getActiveTrack();
	
	switch (i)
	{
	case 4:
		for (auto& event : track->getEvents())
		{
			if (auto ne = std::dynamic_pointer_cast<NoteOnEvent>(event))
			{
				if (ne->getTick() >= time0 && ne->getTick() <= time1)
				{
					if (editType == 0)
					{
						ne->setVelocity(ne->getVelocity() + value);
					} 
					else if (editType == 1)
					{
						ne->setVelocity(ne->getVelocity() - value);
					}
					else if (editType == 2)
					{
						ne->setVelocity(ne->getVelocity() * (value / 100.0));
					}
					else if (editType == 3)
					{
						ne->setVelocity(value);
					}
				}
			}
		}

        mpc.getLayeredScreen()->openScreen<SequencerScreen>();
		break;
	}
}

void EditVelocityScreen::turnWheel(int i)
{

    const auto focusedFieldName = getFocusedFieldNameOrThrow();

	if (focusedFieldName == "edittype")
		setEditType(editType + i);
	else if (focusedFieldName == "value")
		setValue(value + i);
	
	checkAllTimesAndNotes(mpc, i);
}

void EditVelocityScreen::displayTime()
{
	auto sequence = sequencer.lock()->getActiveSequence().get();
	findField("time0")->setTextPadded(SeqUtil::getBarFromTick(sequence, time0) + 1, "0");
	findField("time1")->setTextPadded(SeqUtil::getBeat(sequence, time0) + 1, "0");
	findField("time2")->setTextPadded(SeqUtil::getClock(sequence, time0), "0");
	findField("time3")->setTextPadded(SeqUtil::getBarFromTick(sequence, time1) + 1, "0");
	findField("time4")->setTextPadded(SeqUtil::getBeat(sequence, time1) + 1, "0");
	findField("time5")->setTextPadded(SeqUtil::getClock(sequence, time1), "0");
}

void EditVelocityScreen::displayNotes()
{
    auto track = mpc.getSequencer()->getActiveTrack();
	
	if (track->getBus() == 0)
	{
		findField("note0")->setSize(47, 9);
		findLabel("note1")->Hide(false);
		findField("note1")->Hide(false);
		findField("note0")->setText(StrUtil::padLeft(std::to_string(note0), " ", 3) + "(" + mpc::Util::noteNames()[note0] + u8"\u00D4");
		findField("note1")->setText(StrUtil::padLeft(std::to_string(note1), " ", 3) + "(" + mpc::Util::noteNames()[note1] + u8"\u00D4");
	}
	else
	{
		findField("note0")->setSize(37, 9);
		
		if (note0 == 34)
		{
            findField("note0")->setText("ALL");
		}
		else
		{
            auto program = getProgramOrThrow();
            auto padName = sampler->getPadName(program->getPadIndexFromNote(note0));
            findField("note0")->setText(std::to_string(note0) + "/" + padName);
		}
		
		findLabel("note1")->Hide(true);
		findField("note1")->Hide(true);
	}
}

void EditVelocityScreen::setEditType(int i)
{
	editType = std::clamp(i, 0, 3);
	displayEditType();
}

void EditVelocityScreen::setValue(int i)
{
	value = std::clamp(i, 1, editType == 2 ? 200 : 127);
	displayValue();
}

void EditVelocityScreen::displayValue()
{
	findField("value")->setText(std::to_string(value));
}

void EditVelocityScreen::displayEditType()
{
	findField("edittype")->setText(editTypeNames[editType]);
}
