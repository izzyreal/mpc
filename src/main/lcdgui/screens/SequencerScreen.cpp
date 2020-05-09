#include "SequencerScreen.hpp"

#include <Mpc.hpp>
#include <sequencer/Sequencer.hpp>
#include <sequencer/Sequence.hpp>
#include <sequencer/Track.hpp>
#include <sequencer/TempoChangeEvent.hpp>

#include <sampler/Sampler.hpp>

#include <lcdgui/Parameter.hpp>
#include <lcdgui/Label.hpp>
#include <lcdgui/Field.hpp>

#include <ui/sequencer/window/SequencerWindowGui.hpp>

#include <Util.hpp>

#include <lang/StrUtil.hpp>

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;

SequencerScreen::SequencerScreen(std::vector<std::shared_ptr<Component>> components)
	: ScreenComponent("sequencer"), sequencer(*mpc::Mpc::instance().getSequencer().lock().get()), sampler(*mpc::Mpc::instance().getSampler().lock().get())
{
	for (auto component : components)
	{
		addChild(move(component));
	}

	findLabel("nextsq").lock()->Hide(true);
	findField("nextsq").lock()->Hide(true);

	displaySq();
	displayTr();
	displayOn();
	displayCount();
	displayTiming();
	displayLoop();
	displayRecordingMode();
	displayBars();
	displayNow0();
	displayNow1();
	displayNow2();
	displayTempo();
	displayTempoSource();
	displayTsig();
	displayPgm();
	displayVelo();
	displayBus();
	displayDeviceNumber();
}


void SequencerScreen::displayVelo()
{
	findField("velo").lock()->setText(to_string(sequencer.getActiveTrack().lock()->getVelocityRatio()));
}

void SequencerScreen::displayDeviceNumber()
{
	auto track = sequencer.getActiveTrack().lock();
	if (track->getDevice() == 0) {
		findField("devicenumber").lock()->setText("OFF");
	}
	else {
		if (track->getDevice() >= 17) {
			findField("devicenumber").lock()->setText(to_string(track->getDevice() - 16) + "B");
		}
		else {
			findField("devicenumber").lock()->setText(to_string(track->getDevice()) + "A");
		}
	}
}

vector<string> SequencerScreen::busNames = vector<string>{ "MIDI", "DRUM1", "DRUM2", "DRUM3", "DRUM4" };

void SequencerScreen::displayBus()
{
	findField("tracktype").lock()->setText(busNames[sequencer.getActiveTrack().lock()->getBusNumber()]);
	displayDeviceName();
}

void SequencerScreen::displayBars()
{
	findField("bars").lock()->setText(to_string(sequencer.getActiveSequence().lock()->getLastBar() + 1));
}

void SequencerScreen::displayPgm()
{
	auto track = sequencer.getActiveTrack().lock();
	if (track->getProgramChange() == 0) {
		findField("pgm").lock()->setText("OFF");
	}
	else {
		findField("pgm").lock()->setText(to_string(track->getProgramChange()));
	}
}

void SequencerScreen::displayDeviceName()
{
	auto track = sequencer.getActiveTrack().lock();
	if (track->getBusNumber() != 0) {
		if (track->getDevice() == 0) {
			int pgm = sampler.getDrumBusProgramNumber(track->getBusNumber());
			auto p = dynamic_pointer_cast<mpc::sampler::Program>(sampler.getProgram(pgm).lock());
			findLabel("devicename").lock()->setText(p->getName());
		}
		else {
			findLabel("devicename").lock()->setText(sequencer.getActiveSequence().lock()->getDeviceName(track->getDevice()));
		}
	}
	else if (track->getBusNumber() == 0) {
		if (track->getDevice() == 0) {
			findLabel("devicename").lock()->setText("NewPgm-A");
		}
		else {
			findLabel("devicename").lock()->setText(sequencer.getActiveSequence().lock()->getDeviceName(track->getDevice()));
		}
	}
}

void SequencerScreen::displayTempo()
{
	displayTempoLabel();
	auto tempo = sequencer.getTempo().toString();
	tempo = Util::replaceDotWithSmallSpaceDot(tempo);
	findField("tempo").lock()->setText(tempo);
}

void SequencerScreen::displayTempoLabel()
{
	auto currentRatio = -1;
	for (auto& tce : sequencer.getActiveSequence().lock()->getTempoChangeEvents()) {
		auto lTce = tce.lock();
		if (lTce->getTick() > sequencer.getTickPosition()) {
			break;
		}
		currentRatio = lTce->getRatio();
	}
	if (currentRatio != 1000) {
		findLabel("tempo").lock()->setText(u8"c\u00C0:");
	}
	else {
		findLabel("tempo").lock()->setText(u8" \u00C0:");
	}
}

void SequencerScreen::displayTempoSource()
{
	findField("temposource").lock()->setText(sequencer.isTempoSourceSequenceEnabled() ? "(SEQ)" : "(MAS)");
}


void SequencerScreen::displaySq()
{
	string result{ "" };
	auto lSequencer = mpc::Mpc::instance().getSequencer().lock();
	if (lSequencer->isPlaying()) {
		result.append(moduru::lang::StrUtil::padLeft(to_string(lSequencer->getCurrentlyPlayingSequenceIndex() + 1), "0", 2));
		result.append("-");
		result.append(lSequencer->getCurrentlyPlayingSequence().lock()->getName());
		findField("sq").lock()->setText(result);
	}
	else {
		result.append(moduru::lang::StrUtil::padLeft(to_string(lSequencer->getActiveSequenceIndex() + 1), "0", 2));
		result.append("-");
		result.append(lSequencer->getActiveSequence().lock()->getName());
		findField("sq").lock()->setText(result);
	}
}


void SequencerScreen::displayNow0()
{
	findField("now0").lock()->setTextPadded(sequencer.getCurrentBarNumber() + 1, "0");
}

void SequencerScreen::displayNow1()
{
	findField("now1").lock()->setTextPadded(sequencer.getCurrentBeatNumber() + 1, "0");
}

void SequencerScreen::displayNow2()
{
	findField("now2").lock()->setTextPadded(sequencer.getCurrentClockNumber(), "0");
}

void SequencerScreen::displayRecordingMode()
{
	findField("recordingmode").lock()->setText(sequencer.isRecordingModeMulti() ? "M" : "S");
}

void SequencerScreen::displayTsig()
{
	string result = "";
	auto seq = sequencer.getActiveSequence().lock();
	auto ts = seq->getTimeSignature();
	result.append(to_string(ts.getNumerator()));
	result.append("/");
	result.append(to_string(ts.getDenominator()));
	findField("tsig").lock()->setText(mpc::Util::distributeTimeSig(result));
}

void SequencerScreen::displayLoop()
{
	auto seq = sequencer.getActiveSequence();
	findField("loop").lock()->setText(seq.lock()->isLoopEnabled() ? "ON" : "OFF");
}

void SequencerScreen::displayOn() {
	findField("on").lock()->setText(sequencer.getActiveTrack().lock()->isOn() ? "YES" : "NO");
}

void SequencerScreen::displayTr() {
	
	string result = moduru::lang::StrUtil::padLeft(to_string(sequencer.getActiveTrackIndex() + 1), "0", 2);
	result.append("-");
	result.append(sequencer.getActiveTrack().lock()->getName());
	findField("tr").lock()->setText(result);
}

void SequencerScreen::displayCount() {
	findField("count").lock()->setText(sequencer.isCountEnabled() ? "ON" : "OFF");
}

vector<string> SequencerScreen::timingCorrectNames = vector<string>{ "OFF", "1/8", "1/8(3)", "1/16", "1/16(3)", "1/32", "1/32(3)" };

void SequencerScreen::displayTiming() {
	findField("timing").lock()->setText(timingCorrectNames[mpc::Mpc::instance().getUis().lock()->getSequencerWindowGui()->getNoteValue()]);
}
