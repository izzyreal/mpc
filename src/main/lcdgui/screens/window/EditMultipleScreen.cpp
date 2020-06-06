#include "EditMultipleScreen.hpp"

#include <sequencer/ChannelPressureEvent.hpp>
#include <sequencer/ControlChangeEvent.hpp>
#include <sequencer/Event.hpp>
#include <sequencer/Track.hpp>
#include <sequencer/NoteEvent.hpp>
#include <sequencer/PolyPressureEvent.hpp>
#include <sequencer/ProgramChangeEvent.hpp>

#include <lcdgui/screens/StepEditorScreen.hpp>

using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui;
using namespace mpc::sequencer;
using namespace moduru::lang;
using namespace std;

EditMultipleScreen::EditMultipleScreen(const int layerIndex)
	: ScreenComponent("edit-multiple", layerIndex)
{
}

void EditMultipleScreen::open()
{
	xPosSingle = 60;
	yPosSingle = 25;
}

void EditMultipleScreen::function(int i)
{
	BaseControls::function(i);
	auto stepEditorScreen = dynamic_pointer_cast<StepEditorScreen>(Screens::getScreenComponent("step"));

	auto selectedEvent = stepEditorScreen->getSelectedEvent();
	string paramLetter = stepEditorScreen->getSelectedParameterLetter();
	
	switch (i)
	{
	case 4:
		if (dynamic_pointer_cast<NoteEvent>(selectedEvent.lock()) && track.lock()->getBusNumber() != 0)
		{
			if (paramLetter.compare("a") == 0)
			{
				checkNotes();
			}
			else if (paramLetter.compare("b") == 0)
			{
				for (auto& event : stepEditorScreen->getSelectedEvents())
				{
					if (dynamic_pointer_cast<NoteEvent>(event.lock()))
					{
						dynamic_pointer_cast<NoteEvent>(event.lock())->setVariationTypeNumber(stepEditorScreen->getChangeVariationTypeNumber());
					}
				}
			}
			else if (paramLetter.compare("c") == 0)
			{
				for (auto& event : stepEditorScreen->getSelectedEvents())
				{
					if (dynamic_pointer_cast<NoteEvent>(event.lock()))
					{
						dynamic_pointer_cast<NoteEvent>(event.lock())->setVariationValue(stepEditorScreen->getChangeVariationValue());
					}
				}
			}
			else if (paramLetter.compare("d") == 0)
			{
				checkFiveParameters();
			}
			else if (paramLetter.compare("e") == 0)
			{
				checkThreeParameters();
			}
		}

		if (dynamic_pointer_cast<NoteEvent>(selectedEvent.lock()) && track.lock()->getBusNumber() == 0)
		{
			if (paramLetter.compare("a") == 0)
			{
				checkNotes();
			}
			else if (paramLetter.compare("b") == 0)
			{
				checkFiveParameters();
			}
			else if (paramLetter.compare("c") == 0)
			{
				checkThreeParameters();
			}
		}

		if (dynamic_pointer_cast<ControlChangeEvent>(selectedEvent.lock()))
		{
			if (paramLetter.compare("a") == 0)
			{
				checkFiveParameters();
			}
			else if (paramLetter.compare("b") == 0)
			{
				checkThreeParameters();
			}
		}

		if (dynamic_pointer_cast<ProgramChangeEvent>(selectedEvent.lock()) || dynamic_pointer_cast<ChannelPressureEvent>(selectedEvent.lock()))
		{
			checkFiveParameters();
		}

		if (dynamic_pointer_cast<PolyPressureEvent>(selectedEvent.lock()))
		{
			if (paramLetter.compare("a") == 0)
			{
				checkFiveParameters();
			}
			else if (paramLetter.compare("b") == 0)
			{
				checkThreeParameters();
			}

		}

		stepEditorScreen->clearSelection();
		ls.lock()->openScreen("step");
	}
}

void EditMultipleScreen::turnWheel(int i)
{
	init();

	auto stepEditorScreen = dynamic_pointer_cast<StepEditorScreen>(Screens::getScreenComponent("step"));
	auto event = stepEditorScreen->getSelectedEvent();

	string paramLetter = stepEditorScreen->getSelectedParameterLetter();

	if (param.compare("value0") == 0)
	{
		if (dynamic_pointer_cast<NoteEvent>(event.lock()) && track.lock()->getBusNumber() != 0)
		{
			if (paramLetter.compare("a") == 0)
			{
				if (stepEditorScreen->getChangeNoteToNumber() == 98)
				{
					return;
				}
				stepEditorScreen->setChangeNoteToNumber(stepEditorScreen->getChangeNoteToNumber() + i);
			}
			else if (paramLetter.compare("b") == 0)
			{
				stepEditorScreen->setChangeVariationTypeNumber(stepEditorScreen->getChangeVariationTypeNumber() + i);
			}
			else if (paramLetter.compare("c") == 0)
			{
				stepEditorScreen->setChangeVariationValue(stepEditorScreen->getChangeVariationValue() + i);
			}
			else if (paramLetter.compare("d") == 0 || paramLetter.compare("e") == 0)
			{
				setEditTypeNumber(editTypeNumber + i);
			}
		}
		else if (dynamic_pointer_cast<NoteEvent>(event.lock()) && track.lock()->getBusNumber() == 0)
		{
			if (paramLetter.compare("a") == 0)
			{
				stepEditorScreen->setChangeNoteToNumber(stepEditorScreen->getChangeNoteToNumber() + i);
			}
			else if (paramLetter.compare("b") == 0 || paramLetter.compare("c") == 0)
			{
				setEditTypeNumber(editTypeNumber + i);
			}
		}
		else if (dynamic_pointer_cast<ProgramChangeEvent>(event.lock())
			|| dynamic_pointer_cast<PolyPressureEvent>(event.lock())
			|| dynamic_pointer_cast<ChannelPressureEvent>(event.lock())
			|| dynamic_pointer_cast<ControlChangeEvent>(event.lock()))
		{
			setEditTypeNumber(editTypeNumber + i);
		}
	}
	else if (param.compare("value1") == 0)
	{
		stepEditorScreen->setEditValue(stepEditorScreen->getEditValue() + 1);
	}
}

void EditMultipleScreen::checkThreeParameters()
{
	auto stepEditorScreen = dynamic_pointer_cast<StepEditorScreen>(Screens::getScreenComponent("step"));

	for (auto& event : stepEditorScreen->getSelectedEvents())
	{

		auto note = dynamic_pointer_cast<NoteEvent>(event.lock());
		auto controlChange = dynamic_pointer_cast<ControlChangeEvent>(event.lock());
		auto polyPressure = dynamic_pointer_cast<PolyPressureEvent>(event.lock());

		if (note)
		{
			note->setVelocity(stepEditorScreen->getEditValue());
		}
		else if (controlChange)
		{
			controlChange->setAmount(stepEditorScreen->getEditValue());
		}
		else if (polyPressure)
		{
			polyPressure->setAmount(stepEditorScreen->getEditValue());
		}
	}
}

void EditMultipleScreen::checkFiveParameters()
{
	auto stepEditorScreen = dynamic_pointer_cast<StepEditorScreen>(Screens::getScreenComponent("step"));
	for (auto& event : stepEditorScreen->getSelectedEvents())
	{

		auto note = dynamic_pointer_cast<NoteEvent>(event.lock());
		auto programChange = dynamic_pointer_cast<ProgramChangeEvent>(event.lock());
		auto controlChange = dynamic_pointer_cast<ControlChangeEvent>(event.lock());
		auto channelPressure = dynamic_pointer_cast<ChannelPressureEvent>(event.lock());
		auto polyPressure = dynamic_pointer_cast<PolyPressureEvent>(event.lock());

		if (note) {
			note->setDuration(stepEditorScreen->getEditValue());
		}
		else if (programChange) {
			programChange->setProgram(stepEditorScreen->getEditValue());
		}
		else if (controlChange) {
			controlChange->setController(stepEditorScreen->getEditValue());
		}
		else if (channelPressure) {
			channelPressure->setAmount(stepEditorScreen->getEditValue());
		}
		else if (polyPressure) {
			polyPressure->setNote(stepEditorScreen->getEditValue());
		}
	}
}

void EditMultipleScreen::checkNotes()
{
	auto stepEditorScreen = dynamic_pointer_cast<StepEditorScreen>(Screens::getScreenComponent("step"));
	for (auto& event : stepEditorScreen->getSelectedEvents()) {
		auto note = dynamic_pointer_cast<NoteEvent>(event.lock());
		if (note) {
			note->setNote(stepEditorScreen->getChangeNoteToNumber());
		}
	}
}

void EditMultipleScreen::setEditTypeNumber(int i)
{
	if (i < 0 || i > 3)
	{
		return;
	}
	editTypeNumber = i;
	updateEditMultiple();
}

void EditMultipleScreen::updateEditMultiple()
{
	init();

	auto stepEditorScreen = dynamic_pointer_cast<StepEditorScreen>(Screens::getScreenComponent("step"));
	auto event = stepEditorScreen->getSelectedEvent().lock();
	auto letter = stepEditorScreen->getSelectedParameterLetter();

	if (dynamic_pointer_cast<NoteEvent>(event) && track.lock()->getBusNumber() != 0)
	{
		if (letter.compare("a") == 0 || letter.compare("b") == 0 || letter.compare("c") == 0)
		{
			findLabel("value1").lock()->Hide(true);
			findField("value1").lock()->Hide(true);
			findLabel("value0").lock()->Hide(false);
			findLabel("value0").lock()->setLocation(xPosSingle, yPosSingle);
		
			if (letter.compare("a") == 0)
			{
				findLabel("value0").lock()->setText(singleLabels[0]);
				findField("value0").lock()->setSize(6 * 6 + 1, 9);
				findField("value0").lock()->setText(to_string(stepEditorScreen->getChangeNoteToNumber()) + "/" + sampler.lock()->getPadName(program.lock()->getPadIndexFromNote(stepEditorScreen->getChangeNoteToNumber())));
			}
			else if (letter.compare("b") == 0)
			{
				findLabel("value0").lock()->setText(singleLabels[1]);
				findField("value0").lock()->setSize(3 * 6 + 1, 9);
				findField("value0").lock()->setText(noteVariationParameterNames[stepEditorScreen->getChangeVariationTypeNumber()]);
			}
			else if (letter.compare("c") == 0)
			{
				findLabel("value0").lock()->setText(singleLabels[2]);
			
				if (stepEditorScreen->getChangeVariationTypeNumber() == 0)
				{
					findField("value0").lock()->setSize(4 * 6 + 1, 9);
					findField("value0").lock()->setLocation(45, findField("value0").lock()->getY());
					auto noteVarValue = (stepEditorScreen->getChangeVariationValue() * 2) - 128;

					if (noteVarValue < -120)
					{
						noteVarValue = -120;
					}
					else if (noteVarValue > 120)
					{
						noteVarValue = 120;
					}

					if (noteVarValue == 0)
					{
						findField("value0").lock()->setTextPadded(0, " ");
					}
					else if (noteVarValue < 0)
					{
						findField("value0").lock()->setText("-" + StrUtil::padLeft(to_string(abs(noteVarValue)), " ", 3));
					}
					else if (noteVarValue > 0)
					{
						findField("value0").lock()->setText("+" + StrUtil::padLeft(to_string(noteVarValue), " ", 3));
					}
				}

				if (stepEditorScreen->getChangeVariationTypeNumber() == 1 || stepEditorScreen->getChangeVariationTypeNumber() == 2)
				{
					auto noteVarValue = stepEditorScreen->getChangeVariationValue();
				
					if (noteVarValue > 100)
					{
						noteVarValue = 100;
					}

					findField("value0").lock()->setText(StrUtil::padLeft(to_string(noteVarValue), " ", 3));
					findField("value0").lock()->setSize(3 * 6 + 1, 9);
					findField("value0").lock()->setLocation(51, findField("value0").lock()->getY());
				}
				else if (stepEditorScreen->getChangeVariationTypeNumber() == 3)
				{
					findField("value0").lock()->setSize(4 * 6 + 1, 9);
					findField("value0").lock()->setLocation(45, findField("value0").lock()->getY());

					auto noteVarValue = stepEditorScreen->getChangeVariationValue() - 50;
					
					if (noteVarValue > 50)
					{
						noteVarValue = 50;
					}
					
					if (noteVarValue < 0) {
						findField("value0").lock()->setText("-" + StrUtil::padLeft(to_string(abs(noteVarValue)), " ", 2));
					}
					else if (noteVarValue > 0) {
						findField("value0").lock()->setText("+" + StrUtil::padLeft(to_string(noteVarValue), " ", 2));
					}
					else {
						findField("value0").lock()->setTextPadded("0", " ");
					}
				}
			}

			findLabel("value0").lock()->setSize(findLabel("value0").lock()->getText().length() * 6 + 1, 9);
			findField("value0").lock()->Hide(false);
			findField("value0").lock()->setLocation((xPosSingle)+(findLabel("value0").lock()->getW()), yPosSingle);
		}
		else if (letter.compare("d") == 0 || letter.compare("e") == 0)
		{
			updateDouble();
		}
	}

	if (dynamic_pointer_cast<NoteEvent>(event) && track.lock()->getBusNumber() == 0)
	{
		if (letter.compare("a") == 0)
		{
			findLabel("value1").lock()->Hide(true);
			findField("value1").lock()->Hide(true);
			findLabel("value0").lock()->Hide(false);
			findLabel("value0").lock()->setLocation(xPosSingle, yPosSingle);
			findLabel("value0").lock()->setText(singleLabels[0]);
			findField("value0").lock()->setSize(8 * 6 + 1, 9);
			findField("value0").lock()->setText((StrUtil::padLeft(to_string(stepEditorScreen->getChangeNoteToNumber()), " ", 3) + "(" + mpc::ui::Uis::noteNames[stepEditorScreen->getChangeNoteToNumber()]) + ")");
			findLabel("value0").lock()->setSize(findLabel("value0").lock()->GetTextEntryLength() * 6 + 1, 9);
			findField("value0").lock()->Hide(false);
			findField("value0").lock()->setLocation((xPosSingle)+(findLabel("value0").lock()->getW()), yPosSingle);
		}
		else if (letter.compare("b") == 0 || letter.compare("c") == 0)
		{
			updateDouble();
		}
	}
	if (dynamic_pointer_cast<ProgramChangeEvent>(event)
		|| dynamic_pointer_cast<PolyPressureEvent>(event)
		|| dynamic_pointer_cast<ChannelPressureEvent>(event)
		|| dynamic_pointer_cast<ControlChangeEvent>(event))
	{
		updateDouble();
	}
}

void EditMultipleScreen::updateDouble()
{
	auto stepEditorScreen = dynamic_pointer_cast<StepEditorScreen>(Screens::getScreenComponent("step"));

	findLabel("value0").lock()->Hide(false);
	findLabel("value1").lock()->Hide(false);
	findField("value0").lock()->Hide(false);
	findField("value1").lock()->Hide(false);
	findLabel("value0").lock()->setText(doubleLabels[0]);
	findLabel("value1").lock()->setText(doubleLabels[1]);
	findLabel("value0").lock()->setSize(findLabel("value0").lock()->GetTextEntryLength() * 6 + 1, 9);
	findLabel("value0").lock()->setLocation(xPosDouble[0], yPosDouble[0]);
	findLabel("value1").lock()->setSize(findLabel("value1").lock()->GetTextEntryLength() * 6 + 1, 9);
	findLabel("value1").lock()->setLocation(xPosDouble[1], yPosDouble[1]);
	findField("value0").lock()->setLocation((xPosDouble[0] + findLabel("value0").lock()->getW()), yPosDouble[0]);
	findField("value1").lock()->setLocation((xPosDouble[1] + findLabel("value1").lock()->getW()), yPosDouble[1]);
	findField("value0").lock()->setText(editTypeNames[editTypeNumber]);
	findField("value1").lock()->setText(to_string(stepEditorScreen->getEditValue()));
	findField("value0").lock()->setSize(findField("value0").lock()->GetTextEntryLength() * 6 + 1, 9);
	findField("value1").lock()->setSize(findField("value1").lock()->GetTextEntryLength() * 6 + 1, 9);
}
