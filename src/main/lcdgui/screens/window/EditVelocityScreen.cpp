#include "EditVelocityScreen.hpp"

#include <lcdgui/Label.hpp>

#include <sequencer/Event.hpp>
#include <sequencer/Track.hpp>
#include <sequencer/NoteEvent.hpp>
#include <sequencer/SeqUtil.hpp>

#include <ui/sampler/SamplerGui.hpp>

#include <lang/StrUtil.hpp>

using namespace mpc::lcdgui::screens::window;
using namespace mpc::sequencer;

using namespace moduru::lang;

using namespace std;

EditVelocityScreen::EditVelocityScreen(const int& layer)
	: ScreenComponent("editvelocity", layer)
{
}

void EditVelocityScreen::open()
{
	auto samplerGui = Mpc::instance().getUis().lock()->getSamplerGui();
	samplerGui->addObserver(this);

	auto seq = sequencer.lock()->getActiveSequence().lock();

	setTime0(0);
	setTime1(seq->getLastTick());
	
	displayEditType();
	displayValue();
	displayTime();
	displayNotes();
}

void EditVelocityScreen::close()
{
	auto samplerGui = Mpc::instance().getUis().lock()->getSamplerGui();
	samplerGui->deleteObserver(this);
}

void EditVelocityScreen::update(moduru::observer::Observable* observable, nonstd::any message)
{
	auto msg = nonstd::any_cast<string>(message);

	if (msg.compare("padandnote") == 0)
	{
		displayNotes();
	}
}


void EditVelocityScreen::function(int i)
{
	BaseControls::function(i);
	auto swGui = mpc.getUis().lock()->getSequencerWindowGui();
	switch (i)
	{
	case 4:
		for (auto& event : track.lock()->getEvents())
		{
			auto ne = dynamic_pointer_cast<NoteEvent>(event.lock());

			if (ne)
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
		ls.lock()->openScreen("sequencer");
		break;
	}
}

void EditVelocityScreen::turnWheel(int i)
{
	init();
	auto swGui = mpc.getUis().lock()->getSequencerWindowGui();

	if (param.compare("edittype") == 0)
	{
		setEditType(editType + i);
	}
	else if (param.compare("value") == 0)
	{
		setValue(value + i);
	}
	
	checkAllTimesAndNotes(i);
}

void EditVelocityScreen::displayTime()
{
	auto sequence = sequencer.lock()->getActiveSequence().lock().get();
	findField("time0").lock()->setTextPadded(SeqUtil::getBarFromTick(sequence, time0) + 1, "0");
	findField("time1").lock()->setTextPadded(SeqUtil::getBeat(sequence, time0) + 1, "0");
	findField("time2").lock()->setTextPadded(SeqUtil::getClock(sequence, time0), "0");
	findField("time3").lock()->setTextPadded(SeqUtil::getBarFromTick(sequence, time1) + 1, "0");
	findField("time4").lock()->setTextPadded(SeqUtil::getBeat(sequence, time1) + 1, "0");
	findField("time5").lock()->setTextPadded(SeqUtil::getClock(sequence, time1), "0");
}

void EditVelocityScreen::displayNotes()
{
	init();
	auto lSampler = sampler.lock();
	auto samplerGui = mpc::Mpc::instance().getUis().lock()->getSamplerGui();

	if (track.lock()->getBusNumber() == 0) {
		findField("notes0").lock()->setSize(8 * 6, 9);
		findLabel("notes1").lock()->Hide(false);
		findField("notes1").lock()->Hide(false);
		findField("notes0").lock()->setText(moduru::lang::StrUtil::padLeft(to_string(midiNote0), " ", 3) + "(" + mpc::ui::Uis::noteNames[midiNote0] + u8"\u00D4");
		findField("notes1").lock()->setText(moduru::lang::StrUtil::padLeft(to_string(midiNote1), " ", 3) + "(" + mpc::ui::Uis::noteNames[midiNote1] + u8"\u00D4");
	}
	else {
		findField("notes0").lock()->setSize(6 * 6 + 2, 9);
		if (samplerGui->getNote() != 34) {
			findField("notes0").lock()->setText(to_string(samplerGui->getNote()) + "/" + lSampler->getPadName(samplerGui->getPad()));
		}
		else {
			findField("notes0").lock()->setText("ALL");
		}
		findLabel("notes1").lock()->Hide(true);
		findField("notes1").lock()->Hide(true);
	}
}

void EditVelocityScreen::setEditType(int i)
{
	if (i < 0 || i > 3)
	{
		return;
	}

	editType = i;
	displayEditType();
}

void EditVelocityScreen::setValue(int i)
{
	if (i < 1 || i > 200)
	{
		return;
	}

	if (editType != 2 && i > 127)
	{
		return;
	}

	value = i;
	displayValue();
}

void EditVelocityScreen::displayValue()
{
	findField("value").lock()->setText(to_string(value));
}

void EditVelocityScreen::displayEditType()
{
	findField("edittype").lock()->setText(editTypeNames[editType]);
}
