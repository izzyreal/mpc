#include <ui/sequencer/window/StepWindowObserver.hpp>

#include <Mpc.hpp>
#include <Util.hpp>
#include <ui/Uis.hpp>
#include <lcdgui/LayeredScreen.hpp>
#include <lcdgui/Field.hpp>
#include <lcdgui/Label.hpp>
#include <ui/sampler/SamplerGui.hpp>
#include <ui/sequencer/StepEditorGui.hpp>
#include <ui/sequencer/window/SequencerWindowGui.hpp>
#include <sampler/Program.hpp>
#include <sampler/Sampler.hpp>
#include <sequencer/ChannelPressureEvent.hpp>
#include <sequencer/ControlChangeEvent.hpp>
#include <sequencer/Event.hpp>
#include <sequencer/Sequence.hpp>
#include <sequencer/Track.hpp>
#include <sequencer/NoteEvent.hpp>
#include <sequencer/PolyPressureEvent.hpp>
#include <sequencer/ProgramChangeEvent.hpp>
#include <sequencer/Sequencer.hpp>
#include <mpc/MpcSoundPlayerChannel.hpp>

#include <lcdgui/Screens.hpp>
#include <lcdgui/screens/window/TimingCorrectScreen.hpp>

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::ui::sequencer::window;
using namespace mpc::sequencer;
using namespace std;

StepWindowObserver::StepWindowObserver()
{
	
	timingCorrectNames = { "OFF", "1/8", "1/8(3)", "1/16", "1/16(3)", "1/32", "1/32(3)" };
	eventTypeNames = { "NOTE", "PITCH BEND", "CONTROL CHANGE", "PROGRAM CHANGE", "CH PRESSURE", "POLY PRESSURE", "EXCLUSIVE", "MIXER" };
	noteVariationParameterNames = { "Tun", "Dcy", "Atk", "Flt" };
	editTypeNames = { "ADD VALUE", "SUB VALUE", "MULT VAL%", "SET TO VAL" };
	xPosSingle = 60;
	yPosSingle = 25;
	singleLabels = { "Change note to:", "Variation type:", "Variation value:" };
	xPosDouble = { 60, 84 };
	yPosDouble = { 22, 33 };
	doubleLabels = { "Edit type:", "Value:" };

	sequencer = Mpc::instance().getSequencer();
	sampler = Mpc::instance().getSampler();
	seqGui = Mpc::instance().getUis().lock()->getStepEditorGui();
	samplerGui = Mpc::instance().getUis().lock()->getSamplerGui();
	swGui = Mpc::instance().getUis().lock()->getSequencerWindowGui();
	swGui->addObserver(this);
	seqGui->addObserver(this);
	

	auto lSequencer = sequencer.lock();
	auto seqNum = lSequencer->getActiveSequenceIndex();
	auto trackNum = lSequencer->getActiveTrackIndex();
	track = lSequencer->getSequence(seqNum).lock()->getTrack(trackNum);
	auto lSampler = sampler.lock();
	
	if (track.lock()->getBusNumber() != 0)
	{
		mpcSoundPlayerChannel = lSampler->getDrum(track.lock()->getBusNumber() - 1);
		program = dynamic_pointer_cast<mpc::sampler::Program>(lSampler->getProgram(mpcSoundPlayerChannel->getProgram()).lock());
	}
	
	lSequencer->addObserver(this);
	
	auto ls = Mpc::instance().getLayeredScreen().lock();
	auto csn = ls->getCurrentScreenName();

	tcValueField = ls->lookupField("tcvalue");
	eventtypeField = ls->lookupField("eventtype");
	editMultiParam0Label = ls->lookupLabel("value0");
	editMultiParam1Label = ls->lookupLabel("value1");
	editMultiValue0Field = ls->lookupField("value0");
	editMultiValue1Field = ls->lookupField("value1");

	if (csn.compare("step_tc") == 0)
	{
		auto timingCorrectScreen = dynamic_pointer_cast<TimingCorrectScreen>(Screens::getScreenComponent("timingcorrect"));
		auto noteValue = timingCorrectScreen->getNoteValue();

		tcValueField.lock()->setText(timingCorrectNames[noteValue]);
	}
	else if (csn.compare("insertevent") == 0)
	{
		eventtypeField.lock()->setText(eventTypeNames[seqGui->getInsertEventType()]);
	}
	else if (csn.compare("editmultiple") == 0)
	{
		updateEditMultiple();
	}
}

void StepWindowObserver::update(moduru::observer::Observable* o, nonstd::any arg)
{
	string s = nonstd::any_cast<string>(arg);

	if (s.compare("notevalue") == 0) {
		auto timingCorrectScreen = dynamic_pointer_cast<TimingCorrectScreen>(Screens::getScreenComponent("timingcorrect"));
		auto noteValue = timingCorrectScreen->getNoteValue();
		tcValueField.lock()->setText(timingCorrectNames[noteValue]);
	}
	else if (s.compare("eventtype") == 0) {
		eventtypeField.lock()->setText(eventTypeNames[seqGui->getInsertEventType()]);
	}
	else if (s.compare("editmultiple") == 0) {
		updateEditMultiple();
	}

}

void StepWindowObserver::updateEditMultiple()
{
	auto lSampler = sampler.lock();
	auto event = seqGui->getSelectedEvent().lock();
	if (dynamic_pointer_cast<NoteEvent>(event) && track.lock()->getBusNumber() != 0) {
		if (seqGui->getParamLetter().compare("a") == 0 || seqGui->getParamLetter().compare("b") == 0 || seqGui->getParamLetter().compare("c") == 0) {
			editMultiParam1Label.lock()->Hide(true);
			editMultiValue1Field.lock()->Hide(true);
			editMultiParam0Label.lock()->Hide(false);
			editMultiParam0Label.lock()->setLocation(xPosSingle, yPosSingle);
			if (seqGui->getParamLetter().compare("a") == 0) {
				editMultiParam0Label.lock()->setText(singleLabels[0]);
				editMultiValue0Field.lock()->setSize(6 * 6 + 1, 9);
				editMultiValue0Field.lock()->setText(to_string(seqGui->getChangeNoteToNumber()) + "/" + lSampler->getPadName(program.lock()->getPadNumberFromNote(seqGui->getChangeNoteToNumber())));
			}
			else if (seqGui->getParamLetter().compare("b") == 0) {
				editMultiParam0Label.lock()->setText(singleLabels[1]);
				editMultiValue0Field.lock()->setSize(3 * 6 + 1, 9);
				editMultiValue0Field.lock()->setText(noteVariationParameterNames[seqGui->getChangeVariationTypeNumber()]);
			}
			else if (seqGui->getParamLetter().compare("c") == 0) {
				editMultiParam0Label.lock()->setText(singleLabels[2]);
				if (seqGui->getChangeVariationTypeNumber() == 0) {
					editMultiValue0Field.lock()->setSize(4 * 6 + 1, 9);
					editMultiValue0Field.lock()->setLocation(45, editMultiValue0Field.lock()->getY());
					auto noteVarValue = (seqGui->getChangeVariationValue() * 2) - 128;

					if (noteVarValue < -120) {
						noteVarValue = -120;
					}
					else if (noteVarValue > 120) {
						noteVarValue = 120;
					}

					if (noteVarValue == 0) {
						editMultiValue0Field.lock()->setTextPadded(0, " ");
					}
					else if (noteVarValue < 0) {
						editMultiValue0Field.lock()->setText("-" + moduru::lang::StrUtil::padLeft(to_string(abs(noteVarValue)), " ", 3));
					}
					else if (noteVarValue > 0) {
						editMultiValue0Field.lock()->setText("+" + moduru::lang::StrUtil::padLeft(to_string(noteVarValue), " ", 3));
					}
				}

				if (seqGui->getChangeVariationTypeNumber() == 1 || seqGui->getChangeVariationTypeNumber() == 2) {
					auto noteVarValue = seqGui->getChangeVariationValue();
					if (noteVarValue > 100)
						noteVarValue = 100;

					editMultiValue0Field.lock()->setText(moduru::lang::StrUtil::padLeft(to_string(noteVarValue), " ", 3));
					editMultiValue0Field.lock()->setSize(3 * 6 + 1, 9);
					editMultiValue0Field.lock()->setLocation(51, editMultiValue0Field.lock()->getY());
				}
				else if (seqGui->getChangeVariationTypeNumber() == 3) {
					editMultiValue0Field.lock()->setSize(4 * 6 + 1, 9);
					editMultiValue0Field.lock()->setLocation(45, editMultiValue0Field.lock()->getY());
					auto noteVarValue = seqGui->getChangeVariationValue() - 50;
					if (noteVarValue > 50)
						noteVarValue = 50;

					if (noteVarValue < 0) {
						editMultiValue0Field.lock()->setText("-" + moduru::lang::StrUtil::padLeft(to_string(abs(noteVarValue)), " ", 2));
					}
					if (noteVarValue > 0) {
						editMultiValue0Field.lock()->setText("+" + moduru::lang::StrUtil::padLeft(to_string(noteVarValue), " ", 2));
					}
					if (noteVarValue == 0) {
						editMultiValue0Field.lock()->setTextPadded("0", " ");
					}
				}
			}
			editMultiParam0Label.lock()->setSize(editMultiParam0Label.lock()->getText().length() * 6 + 1, 9);
			editMultiValue0Field.lock()->Hide(false);
			editMultiValue0Field.lock()->setLocation((xPosSingle) + (editMultiParam0Label.lock()->getW()), yPosSingle);
		}
		else if (seqGui->getParamLetter().compare("d") == 0 || seqGui->getParamLetter().compare("e") == 0) {
			updateDouble();
		}
	}

	if (dynamic_pointer_cast<NoteEvent>(event) && track.lock()->getBusNumber() == 0) {
		if (seqGui->getParamLetter().compare("a") == 0) {
			editMultiParam1Label.lock()->Hide(true);
			editMultiValue1Field.lock()->Hide(true);
			editMultiParam0Label.lock()->Hide(false);
			editMultiParam0Label.lock()->setLocation(xPosSingle, yPosSingle);
			editMultiParam0Label.lock()->setText(singleLabels[0]);
			editMultiValue0Field.lock()->setSize(8 * 6 + 1, 9);
			editMultiValue0Field.lock()->setText((moduru::lang::StrUtil::padLeft(to_string(seqGui->getChangeNoteToNumber()), " ", 3) + "(" + mpc::ui::Uis::noteNames[seqGui->getChangeNoteToNumber()]) + ")");
			editMultiParam0Label.lock()->setSize(editMultiParam0Label.lock()->GetTextEntryLength() * 6 + 1, 9);
			editMultiValue0Field.lock()->Hide(false);
			editMultiValue0Field.lock()->setLocation((xPosSingle) + (editMultiParam0Label.lock()->getW()), yPosSingle);
		}
		else if (seqGui->getParamLetter().compare("b") == 0 || seqGui->getParamLetter().compare("c") == 0) {
			updateDouble();
		}
	}
	if (dynamic_pointer_cast<ProgramChangeEvent>(event)
		|| dynamic_pointer_cast<PolyPressureEvent>(event)
		|| dynamic_pointer_cast<ChannelPressureEvent>(event)
		|| dynamic_pointer_cast<ControlChangeEvent>(event)) {
		updateDouble();
	}
}

void StepWindowObserver::updateDouble()
{
    editMultiParam0Label.lock()->Hide(false);
    editMultiParam1Label.lock()->Hide(false);
    editMultiValue0Field.lock()->Hide(false);
    editMultiValue1Field.lock()->Hide(false);
    editMultiParam0Label.lock()->setText(doubleLabels[0]);
    editMultiParam1Label.lock()->setText(doubleLabels[1]);
	editMultiParam0Label.lock()->setSize(editMultiParam0Label.lock()->GetTextEntryLength() * 6 + 1, 9);
    editMultiParam0Label.lock()->setLocation(xPosDouble[0], yPosDouble[0]);
    editMultiParam1Label.lock()->setSize(editMultiParam1Label.lock()->GetTextEntryLength() * 6 + 1, 9);
    editMultiParam1Label.lock()->setLocation(xPosDouble[1], yPosDouble[1]);
    editMultiValue0Field.lock()->setLocation((xPosDouble[0] + editMultiParam0Label.lock()->getW()), yPosDouble[0]);
    editMultiValue1Field.lock()->setLocation((xPosDouble[1] + editMultiParam1Label.lock()->getW()), yPosDouble[1]);
    editMultiValue0Field.lock()->setText(editTypeNames[seqGui->getEditTypeNumber()]);
	editMultiValue1Field.lock()->setText(to_string(seqGui->getEditValue()));
    editMultiValue0Field.lock()->setSize(editMultiValue0Field.lock()->GetTextEntryLength() * 6 + 1, 9);
    editMultiValue1Field.lock()->setSize(editMultiValue1Field.lock()->GetTextEntryLength() * 6 + 1, 9);
}

StepWindowObserver::~StepWindowObserver() {
	seqGui->deleteObserver(this);
	sequencer.lock()->deleteObserver(this);
	swGui->deleteObserver(this);
}
