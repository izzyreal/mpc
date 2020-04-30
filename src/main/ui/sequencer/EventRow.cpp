#include "EventRow.hpp"

#include <Mpc.hpp>
#include <Util.hpp>
#include <lcdgui/LayeredScreen.hpp>
#include <lcdgui/HorizontalBar.hpp>
#include <lcdgui/Field.hpp>
#include <ui/Uis.hpp>
#include <ui/sampler/SamplerGui.hpp>
#include <lcdgui/SelectedEventBar.hpp>
#include <sampler/Pad.hpp>
#include <sampler/Program.hpp>
#include <sampler/Sampler.hpp>
#include <sequencer/ChannelPressureEvent.hpp>
#include <sequencer/ControlChangeEvent.hpp>
#include <sequencer/EmptyEvent.hpp>
#include <sequencer/Event.hpp>
#include <sequencer/MixerEvent.hpp>
#include <sequencer/NoteEvent.hpp>
#include <sequencer/PitchBendEvent.hpp>
#include <sequencer/PolyPressureEvent.hpp>
#include <sequencer/ProgramChangeEvent.hpp>
#include <sequencer/SystemExclusiveEvent.hpp>
#include <mpc/MpcSoundPlayerChannel.hpp>

#include <file/File.hpp>

#include <cmath>
#include <lang/StrUtil.hpp>

using namespace mpc::ui::sequencer;
using namespace std;

EventRow::EventRow(int bus, weak_ptr<mpc::sequencer::Event> e, int rowNumber)
{
	
	letters = vector<string>{ "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m" };
	noteVarParamNames = vector<string>{ "Tun", "Dcy", "Atk", "Flt" };
	drumNoteEventLabels = vector<string>{ ">N: ", "", ":", "D:", "V:" };
	drumNoteEventSizes = vector<int>{ 6, 3, 4, 4, 3 };
	drumNoteEventXPos = vector<int>{ 0, 66, 85, 120, 162 };
	midiNoteEventLabels = vector<string>{ ">Note:", "D:", "V:" };
	midiNoteEventSizes = vector<int>{ 8, 4, 3 };
	midiNoteEventXPos = vector<int>{ 0, 98, 144 };
	miscEventLabels = vector<string>{ ">BEND          :", ":" };
	miscEventSizes = vector<int>{ 5, 0 };
	miscEventXPos = vector<int>{ 0, 168 };
	polyPressureEventLabels = vector<string>{ ">POLY PRESSURE :", ":" };
	polyPressureEventSizes = vector<int>{ 8, 3 };
	polyPressureEventXPos = vector<int>{ 0, 168 };
	sysexEventLabels = vector<string>{ ">Exclusive:", "" };
	sysexEventSizes = vector<int>{ 2, 2 };
	sysexEventXPos = vector<int>{ 0, 81 };
	emptyEventLabels = vector<string>{ "" };
	emptyEventSizes = vector<int>{ 1 };
	emptyEventXPos = vector<int>{ 6 };
	channelPressureEventLabels = vector<string>{ ">CH PRESSURE   :            :" };
	channelPressureEventSizes = vector<int>{ 3 };
	channelPressureEventXPos = vector<int>{ 0 };
	selected = false;
	controlChangeEventLabels = vector<string>{ ">CONTROL CHANGE:", ":" };
	controlChangeEventSizes = vector<int>{ 12, 3 };
	controlChangeEventXPos = vector<int>{ 0, 168 };
	mixerParamNames = vector<string>{ "STEREO LEVEL", "STEREO PAN  ", "FXsend LEVEL", "INDIV LEVEL" };
	mixerEventLabels = vector<string>{ ">", "N:", "L:" };
	mixerEventSizes = vector<int>{ 12, 6, 3 };
	mixerEventXPos = vector<int>{ 0, 96, 162 };
	samplerGui = Mpc::instance().getUis().lock()->getSamplerGui();
	sampler = Mpc::instance().getSampler();
	auto lSampler = sampler.lock();
	if (bus != 0) {
		mpcSoundPlayerChannel = lSampler->getDrum(bus - 1);
		program = dynamic_pointer_cast<mpc::sampler::Program>(lSampler->getProgram(mpcSoundPlayerChannel->getProgram()).lock());
	}
	midi = false;
	event = e;
	this->rowNumber = rowNumber;
	auto ls = Mpc::instance().getLayeredScreen().lock();
	horizontalBar = ls->getHorizontalBarsStepEditor()[rowNumber];
	horizontalBar.lock()->Hide(false);
	selectedEventBar = ls->getSelectedEventBarsStepEditor()[rowNumber];
	selectedEventBar.lock()->Hide(false);
	eventRow.push_back(horizontalBar);
	eventRow.push_back(selectedEventBar);
	initLabelsAndFields();
}

vector<string> EventRow::controlNames = vector<string>{ "BANK SEL MSB", "MOD WHEEL", "BREATH CONT", "03", "FOOT CONTROL", "PORTA TIME", "DATA ENTRY", "MAIN VOLUME", "BALANCE", "09", "PAN", "EXPRESSION", "EFFECT 1", "EFFECT 2", "14", "15", "GEN.PUR. 1", "GEN.PUR. 2", "GEN.PUR. 3", "GEN.PUR. 4", "20", "21", "22", "23", "24", "25", "26", "27", "28", "29", "30", "31", "BANK SEL LSB", "MOD WHEL LSB", "BREATH LSB", "35", "FOOT CNT LSB", "PORT TIME LS", "DATA ENT LSB", "MAIN VOL LSB", "BALANCE LSB", "41", "PAN LSB", "EXPRESS LSB", "EFFECT 1 LSB", "EFFECT 2 MSB", "46", "47", "GEN.PUR.1 LS", "GEN.PUR.2 LS", "GEN.PUR.3 LS", "GEN.PUR.4 LS", "52", "53", "54", "55", "56", "57", "58", "59", "60", "61", "62", "63", "SUSTAIN PDL", "PORTA PEDAL", "SOSTENUTO", "SOFT PEDAL", "LEGATO FT SW", "HOLD 2", "SOUND VARI", "TIMBER/HARMO", "RELEASE TIME", "ATTACK TIME", "BRIGHTNESS", "SOUND CONT 6", "SOUND CONT 7", "SOUND CONT 8", "SOUND CONT 9", "SOUND CONT10", "GEN.PUR. 5", "GEN.PUR. 6", "GEN.PUR. 7", "GEN.PUR. 8", "PORTA CNTRL", "85", "86", "87", "88", "89", "90", "EXT EFF DPTH", "TREMOLO DPTH", "CHORUS DEPTH", " DETUNE DEPTH", "PHASER DEPTH", "DATA INCRE", "DATA DECRE", "NRPN LSB", "NRPN MSB", "RPN LSB", "RPN MSB", "102", "103", "104", "105", "106", "107", "108", "109", "110", "111", "112", "113", "114", "115", "116", "117", "118", "119", "ALL SND OFF", "RESET CONTRL", "LOCAL ON/OFF", "ALL NOTE OFF", "OMNI OFF", "OMNI ON", "MONO MODE ON", "POLY MODE ON" };

void EventRow::setMidi(bool b)
{
    midi = b;
}

void EventRow::init()
{
	if (dynamic_pointer_cast<mpc::sequencer::NoteEvent>(event.lock())) {
		if (midi) {
			setLabelTexts(midiNoteEventLabels);
			setSizeAndLocation(midiNoteEventXPos, midiNoteEventSizes);
			setMidiNoteEventValues();
		}
		if (!midi) {
			setLabelTexts(drumNoteEventLabels);
			setSizeAndLocation(drumNoteEventXPos, drumNoteEventSizes);
			setDrumNoteEventValues();
		}
	}
	else if (dynamic_pointer_cast<mpc::sequencer::EmptyEvent>(event.lock())) {
		setLabelTexts(emptyEventLabels);
		setSizeAndLocation(emptyEventXPos, emptyEventSizes);
		setEmptyEventValues();
	}
	else if (dynamic_pointer_cast<mpc::sequencer::PitchBendEvent>(event.lock())) {
		setLabelTexts(miscEventLabels);
		setSizeAndLocation(miscEventXPos, miscEventSizes);
		setMiscEventValues();
	}
	else if (dynamic_pointer_cast<mpc::sequencer::ProgramChangeEvent>(event.lock())) {
		setLabelTexts(miscEventLabels);
		setSizeAndLocation(miscEventXPos, miscEventSizes);
		setMiscEventValues();
	}
	else if (dynamic_pointer_cast<mpc::sequencer::ControlChangeEvent>(event.lock())) {
		setLabelTexts(controlChangeEventLabels);
		setSizeAndLocation(controlChangeEventXPos, controlChangeEventSizes);
		setControlChangeEventValues();
	}
	else if (dynamic_pointer_cast<mpc::sequencer::ChannelPressureEvent>(event.lock())) {
		setLabelTexts(channelPressureEventLabels);
		setSizeAndLocation(channelPressureEventXPos, channelPressureEventSizes);
		setChannelPressureEventValues();
	}
	else if (dynamic_pointer_cast<mpc::sequencer::PolyPressureEvent>(event.lock())) {
		setLabelTexts(polyPressureEventLabels);
		setSizeAndLocation(polyPressureEventXPos, polyPressureEventSizes);
		setPolyPressureEventValues();
	}
	else if (dynamic_pointer_cast<mpc::sequencer::SystemExclusiveEvent>(event.lock())) {
		setLabelTexts(sysexEventLabels);
		setSizeAndLocation(sysexEventXPos, sysexEventSizes);
		setSystemExclusiveEventValues();
	}
	else if (dynamic_pointer_cast<mpc::sequencer::MixerEvent>(event.lock())) {
		setLabelTexts(mixerEventLabels);
		setSizeAndLocation(mixerEventXPos, mixerEventSizes);
		setMixerEventValues();
	}

	for (int i = 0; i < tfArray.size(); i++) {
		eventRow.push_back(labelArray[i]);
		eventRow.push_back(tfArray[i]);
	}
}

void EventRow::setEmptyEventValues()
{
	tfArray[0].lock()->Hide(false);
	labelArray[0].lock()->Hide(false);
	labelArray[0].lock()->setText("");
	tfArray[0].lock()->setText(" ");
	horizontalBar.lock()->Hide(true);
	tfArray[1].lock()->Hide(true);
	tfArray[2].lock()->Hide(true);
	tfArray[3].lock()->Hide(true);
	tfArray[4].lock()->Hide(true);
	labelArray[1].lock()->Hide(true);
	labelArray[2].lock()->Hide(true);
	labelArray[3].lock()->Hide(true);
	labelArray[4].lock()->Hide(true);
}

void EventRow::setSystemExclusiveEventValues()
{
    if (!event.lock()) return;

    auto see = dynamic_pointer_cast<mpc::sequencer::SystemExclusiveEvent>(event.lock());
    for (int i = 0; i < 2; i++) {
        tfArray[i].lock()->Hide(false);
        labelArray[i].lock()->Hide(false);
    }
    tfArray[0].lock()->setText(moduru::lang::StrUtil::padLeft(to_string(see->getByteA()), "0", 2));
	tfArray[1].lock()->setText(moduru::lang::StrUtil::padLeft(to_string(see->getByteB()), "0", 2));
    horizontalBar.lock()->Hide(true);
    tfArray[2].lock()->Hide(true);
    tfArray[3].lock()->Hide(true);
    tfArray[4].lock()->Hide(true);
    labelArray[2].lock()->Hide(true);
    labelArray[3].lock()->Hide(true);
    labelArray[4].lock()->Hide(true);
}

void EventRow::setPolyPressureEventValues()
{
    if (!event.lock()) return;
    auto ppe = dynamic_pointer_cast<mpc::sequencer::PolyPressureEvent>(event.lock());
    for (int i = 0; i < 2; i++) {
        tfArray[i].lock()->Hide(false);
        labelArray[i].lock()->Hide(false);
    }
    tfArray[0].lock()->setText(moduru::lang::StrUtil::padLeft(to_string(ppe->getNote()), " ", 3) + "(" + mpc::ui::Uis::noteNames[ppe->getNote()] + ")");
    tfArray[1].lock()->setText(moduru::lang::StrUtil::padLeft(to_string(ppe->getAmount()), " ", 3));
	auto lHorizontalBar = horizontalBar.lock();
	lHorizontalBar->setValue(ppe->getAmount());
    lHorizontalBar->Hide(false);
    tfArray[2].lock()->Hide(true);
    tfArray[3].lock()->Hide(true);
    tfArray[4].lock()->Hide(true);
    labelArray[2].lock()->Hide(true);
    labelArray[3].lock()->Hide(true);
    labelArray[4].lock()->Hide(true);
}

void EventRow::setChannelPressureEventValues()
{
    if (!event.lock()) return;

    auto cpe = dynamic_pointer_cast< mpc::sequencer::ChannelPressureEvent>(event.lock());
    tfArray[0].lock()->Hide(false);
    labelArray[0].lock()->Hide(false);
    tfArray[0].lock()->setText(moduru::lang::StrUtil::padLeft(to_string(cpe->getAmount()), " ", 3));
	auto lHorizontalBar = horizontalBar.lock();
	lHorizontalBar->setValue(cpe->getAmount());
    lHorizontalBar->Hide(false);
    tfArray[1].lock()->Hide(true);
    tfArray[2].lock()->Hide(true);
    tfArray[3].lock()->Hide(true);
    tfArray[4].lock()->Hide(true);
    labelArray[1].lock()->Hide(true);
    labelArray[2].lock()->Hide(true);
    labelArray[3].lock()->Hide(true);
    labelArray[4].lock()->Hide(true);
}

void EventRow::setControlChangeEventValues()
{
    if (!event.lock())
        return;

    auto cce = dynamic_pointer_cast< mpc::sequencer::ControlChangeEvent>(event.lock());
    for (int i = 0; i < 2; i++) {
        tfArray[i].lock()->Hide(false);
        labelArray[i].lock()->Hide(false);
    }
    tfArray[0].lock()->setText(controlNames[cce->getController()]);
    tfArray[1].lock()->setText(moduru::lang::StrUtil::padLeft(to_string(cce->getAmount()), " ", 3));
	auto lHorizontalBar = horizontalBar.lock();
	lHorizontalBar->setValue(cce->getAmount());
    lHorizontalBar->Hide(false);
    tfArray[2].lock()->Hide(true);
    tfArray[3].lock()->Hide(true);
    tfArray[4].lock()->Hide(true);
    labelArray[2].lock()->Hide(true);
    labelArray[3].lock()->Hide(true);
    labelArray[4].lock()->Hide(true);
}

void EventRow::setMiscEventValues()
{
    if (!event.lock()) return;

    auto parameterValue = 0;
    if(dynamic_pointer_cast< mpc::sequencer::PitchBendEvent>(event.lock()) != nullptr) {
        parameterValue = dynamic_pointer_cast< mpc::sequencer::PitchBendEvent>(event.lock())->getAmount();
    }
    if(dynamic_pointer_cast< mpc::sequencer::ProgramChangeEvent>(event.lock()) != nullptr) {
        parameterValue = dynamic_pointer_cast< mpc::sequencer::ProgramChangeEvent>(event.lock())->getProgram();
        labelArray[0].lock()->setText(">PROGRAM CHANGE:");
        tfArray[0].lock()->setSize(3 * 6 + 2, 9);
    }
    for (int i = 0; i < 2; i++) {
        tfArray[i].lock()->Hide(false);
        labelArray[i].lock()->Hide(false);
    }
    tfArray[0].lock()->setText(moduru::lang::StrUtil::padLeft(to_string(parameterValue), " ", 3));
    if(dynamic_pointer_cast< mpc::sequencer::PitchBendEvent>(event.lock()) != nullptr) {
        if(parameterValue > 0) {
            tfArray[0].lock()->setText("+" + moduru::lang::StrUtil::padLeft(to_string(parameterValue), " ", 4));
        }
        if(parameterValue < 0) {
            tfArray[0].lock()->setText("-" + moduru::lang::StrUtil::padLeft(to_string(abs(parameterValue)), " ", 4));
        }
        if(parameterValue == 0) {
            tfArray[0].lock()->setText("    0");
        }
    }
    horizontalBar.lock()->Hide(true);
    tfArray[1].lock()->Hide(true);
    tfArray[2].lock()->Hide(true);
    tfArray[3].lock()->Hide(true);
    tfArray[4].lock()->Hide(true);
    labelArray[2].lock()->Hide(true);
    labelArray[3].lock()->Hide(true);
    labelArray[4].lock()->Hide(true);
}

void EventRow::setMixerEventValues()
{
	if (!event.lock()) return;
	auto lSampler = sampler.lock();
	auto me = dynamic_pointer_cast<mpc::sequencer::MixerEvent>(event.lock());
	for (int i = 0; i < 3; i++) {
		tfArray[i].lock()->Hide(false);
		labelArray[i].lock()->Hide(false);
	}
	tfArray[0].lock()->setText(mixerParamNames[me->getParameter()]);
	auto nn = program.lock()->getPad(me->getPad())->getNote();
	tfArray[1].lock()->setText(string(nn == 34 ? "--" : to_string(nn)) + "/" + lSampler->getPadName(me->getPad()));
	if (me->getParameter() == 1) {
		labelArray[2].lock()->setText("P:");
		auto panning = "L";
		if (me->getValue() > 50) panning = "R";

		tfArray[2].lock()->setText(panning + moduru::lang::StrUtil::padLeft(to_string(abs(me->getValue() - 50)), " ", 2));
		if (me->getValue() == 50) {
			tfArray[2].lock()->setText("0  ");
		}
	}
	else {
		labelArray[2].lock()->setText("L:");
		tfArray[2].lock()->setText(moduru::lang::StrUtil::padLeft(to_string(me->getValue()), " ", 3));
	}
	auto lHorizontalBar = horizontalBar.lock();
	lHorizontalBar->setValue(me->getValue() * 1.27);
	lHorizontalBar->Hide(false);
	tfArray[3].lock()->Hide(true);
	tfArray[4].lock()->Hide(true);
	labelArray[3].lock()->Hide(true);
	labelArray[4].lock()->Hide(true);
}

void EventRow::setDrumNoteEventValues()
{
	if (!event.lock())
		return;
	auto lSampler = sampler.lock();
	auto ne = dynamic_pointer_cast<mpc::sequencer::NoteEvent>(event.lock());
	for (int i = 0; i < 5; i++) {
		tfArray[i].lock()->Hide(false);
		labelArray[i].lock()->Hide(false);
	}
	if (ne->getNote() < 35 || ne->getNote() > 98) {
		tfArray[0].lock()->setText("--/OFF");
	}
	else {
		tfArray[0].lock()->setText(to_string(ne->getNote()) + "/" + lSampler->getPadName(program.lock()->getPadNumberFromNote(ne->getNote())));
	}
	tfArray[1].lock()->setText(noteVarParamNames[ne->getVariationTypeNumber()]);
	if (ne->getVariationTypeNumber() == 0) {
		tfArray[2].lock()->setSize(4 * 6 + 1, 9);
		tfArray[2].lock()->setLocation(90, tfArray[2].lock()->getY());
		auto noteVarValue = (ne->getVariationValue() * 2) - 128;
		if (noteVarValue < -120) noteVarValue = -120;
		if (noteVarValue > 120) noteVarValue = 120;
		if (noteVarValue == 0) tfArray[2].lock()->setText("   0");
		if (noteVarValue < 0) tfArray[2].lock()->setText("-" + moduru::lang::StrUtil::padLeft(to_string(abs(noteVarValue)), " ", 3));
		if (noteVarValue > 0) tfArray[2].lock()->setText("+" + moduru::lang::StrUtil::padLeft(to_string(noteVarValue), " ", 3));
	}
	else if (ne->getVariationTypeNumber() == 1 || ne->getVariationTypeNumber() == 2) {
		auto noteVarValue = ne->getVariationValue();
		if (noteVarValue > 100) noteVarValue = 100;
		tfArray[2].lock()->setText(moduru::lang::StrUtil::padLeft(to_string(noteVarValue), " ", 3));
		tfArray[2].lock()->setSize(3 * 6 + 1, 9);
		tfArray[2].lock()->setLocation(90 + 6, tfArray[2].lock()->getY());
	}
	else if (ne->getVariationTypeNumber() == 3) {
		tfArray[2].lock()->setSize(4 * 6 + 1, 9);
		tfArray[2].lock()->setLocation(90, tfArray[2].lock()->getY());
		auto noteVarValue = ne->getVariationValue() - 50;
		if (noteVarValue > 50) noteVarValue = 50;
		if (noteVarValue < 0) tfArray[2].lock()->setText("-" + moduru::lang::StrUtil::padLeft(to_string(abs(noteVarValue)), " ", 2));
		if (noteVarValue > 0) tfArray[2].lock()->setText("+" + moduru::lang::StrUtil::padLeft(to_string(noteVarValue), " ", 2));
		if (noteVarValue == 0) tfArray[2].lock()->setText("  0");
	}
	tfArray[3].lock()->setText(moduru::lang::StrUtil::padLeft(to_string(ne->getDuration()), " ", 4));
	tfArray[4].lock()->setText(moduru::lang::StrUtil::padLeft(to_string(ne->getVelocity()), " ", 3));
	auto lHorizontalBar = horizontalBar.lock(); 
	lHorizontalBar->setValue(ne->getVelocity());
	lHorizontalBar->Hide(false);
}

void EventRow::setMidiNoteEventValues()
{
	if (!event.lock()) return;
	auto ne = dynamic_pointer_cast<mpc::sequencer::NoteEvent>(event.lock());
	for (int i = 0; i < 3; i++) {
		tfArray[i].lock()->Hide(false);
		labelArray[i].lock()->Hide(false);
	}
	tfArray[0].lock()->setText(moduru::lang::StrUtil::padLeft(to_string(ne->getNote()), " ", 3) + "(" + mpc::ui::Uis::noteNames[ne->getNote()] + ")");
	tfArray[1].lock()->setText(moduru::lang::StrUtil::padLeft(to_string(ne->getDuration()), " ", 4));
	tfArray[2].lock()->setText(to_string(ne->getVelocity()));
	auto lHorizontalBar = horizontalBar.lock(); 
	lHorizontalBar->setValue(ne->getVelocity());
	lHorizontalBar->Hide(false);
	tfArray[3].lock()->Hide(true);
	tfArray[4].lock()->Hide(true);
	labelArray[3].lock()->Hide(true);
	labelArray[4].lock()->Hide(true);
}

void EventRow::initLabelsAndFields()
{
	auto ls = Mpc::instance().getLayeredScreen().lock();
	for (int i = 0; i < 5; i++) {
		auto candidate = ls->lookupField(letters[i] + to_string(rowNumber));
		tfArray.push_back(candidate);
		auto label = ls->lookupLabel(letters[i] + to_string(rowNumber));
		labelArray.push_back(label);
	}
	horizontalBar.lock()->Hide(true);
	setColors();
}

void EventRow::setColors()
{
	auto ls = Mpc::instance().getLayeredScreen().lock();
	for (int i = 0; i < 5; i++) {
		if (selected) {
			selectedEventBar.lock()->Hide(false);
			selectedEventBar.lock()->SetDirty();
			labelArray[i].lock()->setInverted(true);
			labelArray[i].lock()->setOpaque(true);
			if (ls->getFocus().compare(tfArray[i].lock()->getName()) == 0) {
				tfArray[i].lock()->setInverted(false);
				//tfArray[i].lock()->setOpaque(false);
			}
			else {
				tfArray[i].lock()->setInverted(true);
			}
		}
		else {
			selectedEventBar.lock()->Hide(true);
			labelArray[i].lock()->setInverted(false);
			//labelArray[i].lock()->setOpaque(false);
			if (tfArray[i].lock()->hasFocus()) {
				tfArray[i].lock()->setInverted(true);
			}
			else {
				tfArray[i].lock()->setInverted(false);
			}
		}
	}
}

void EventRow::setLabelTexts(vector<string> labels)
{
	for (int i = 0; i < labels.size(); i++)
		labelArray[i].lock()->setText(labels[i]);
}

void EventRow::setSizeAndLocation(vector<int> xPosArray, vector<int> sizeArray)
{
	for (int i = 0; i < xPosArray.size(); i++) {
		tfArray[i].lock()->setSize((sizeArray[i] * 6) + 1, 9);
		labelArray[i].lock()->setSize(labelArray[i].lock()->getText().length() * 6, 9);
		labelArray[i].lock()->setLocation(xPosArray[i] - 1, 11 + (rowNumber * 9));
		tfArray[i].lock()->setLocation(xPosArray[i] + (labelArray[i].lock()->getText().length() * 6) - 1, 11 + (rowNumber * 9));
	}
}

vector<weak_ptr<mpc::lcdgui::Component>> EventRow::getEventRow()
{
	return eventRow;
}

void EventRow::setEvent(weak_ptr<mpc::sequencer::Event> event)
{
    this->event = event;
}

void EventRow::setSelected(bool b)
{
    selected = b;
    setColors();
}

bool EventRow::isSelected()
{
    return selected;
}

EventRow::~EventRow() {
	// hide these things exactly when they need to be hidden, so not in this dtor
	//if (horizontalBar.lock())
		//horizontalBar.lock()->Hide(true);
	//if (selectedEventBar.lock())
		//selectedEventBar.lock()->Hide(true);
}
