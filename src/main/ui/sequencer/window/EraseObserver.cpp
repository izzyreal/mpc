#include <ui/sequencer/window/EraseObserver.hpp>

#include <Mpc.hpp>
#include <Util.hpp>
#include <ui/Uis.hpp>
#include <lcdgui/LayeredScreen.hpp>
#include <lcdgui/Field.hpp>
#include <lcdgui/Label.hpp>
#include <ui/sampler/SamplerGui.hpp>
#include <ui/sequencer/window/EraseGui.hpp>
#include <ui/sequencer/window/SequencerWindowGui.hpp>
#include <sampler/Program.hpp>
#include <sampler/Sampler.hpp>
#include <sequencer/Sequence.hpp>
#include <sequencer/Track.hpp>
#include <sequencer/SeqUtil.hpp>
#include <sequencer/Sequencer.hpp>
#include <mpc/MpcSoundPlayerChannel.hpp>

#include <lang/StrUtil.hpp>

using namespace mpc::ui::sequencer::window;
using namespace std;

EraseObserver::EraseObserver(mpc::Mpc* mpc)
{
	typeNames = vector<string>{ "NOTES", "PITCH BEND", "CONTROL", "PROG CHANGE", "CH PRESSURE", "POLY PRESS", "EXCLUSIVE" };
	eraseNames = vector<string>{ "ALL EVENTS", "ALL EXCEPT", "ONLY ERASE" };
	bus = 0;
	eraseGui = mpc->getUis().lock()->getEraseGui();
	swGui = mpc->getUis().lock()->getSequencerWindowGui();
	eraseGui->deleteObservers();
	eraseGui->addObserver(this);
	swGui->deleteObservers();
	swGui->addObserver(this);
	auto ls = mpc->getLayeredScreen().lock();
	trackField = ls->lookupField("track");
	trackNameLabel = ls->lookupLabel("trackname");
	time0Field = ls->lookupField("time0");
	time1Field = ls->lookupField("time1");
	time2Field = ls->lookupField("time2");
	time3Field = ls->lookupField("time3");
	time4Field = ls->lookupField("time4");
	time5Field = ls->lookupField("time5");
	eraseField = ls->lookupField("erase");
	typeField = ls->lookupField("type");
	notes0Field = ls->lookupField("notes0");
	notes0Label = ls->lookupLabel("notes0");
	notes1Field = ls->lookupField("notes1");
	notes1Label = ls->lookupLabel("notes1");
	auto lSequencer = mpc->getSequencer().lock();
	sequence = lSequencer->getActiveSequence().lock().get();
	sampler = mpc->getSampler();
	bus = sequence->getTrack(lSequencer->getActiveTrackIndex()).lock()->getBusNumber();
	int drum = bus - 1;
	auto lSampler = sampler.lock();
	auto mpcSoundPlayerChannel = drum >= 0 ? lSampler->getDrum(drum) : nullptr;
	program = drum >= 0 ? dynamic_pointer_cast<mpc::sampler::Program>(lSampler->getProgram(mpcSoundPlayerChannel->getProgram()).lock()) : weak_ptr<mpc::sampler::Program>();
	displayTrack();
	displayTime();
	displayErase();
	displayType();
	displayNotes();
}

void EraseObserver::update(moduru::observer::Observable* o, nonstd::any arg)
{
	string s = nonstd::any_cast<string>(arg);
	if (s.compare("track") == 0) {
		displayTrack();
	}
	else if (s.compare("time") == 0) {
		displayTime();
	}
	else if (s.compare("erase") == 0) {
		displayErase();
		displayType();
		displayNotes();
	}
	else if (s.compare("type") == 0) {
		displayType();
		displayNotes();
	}
	else if (s.compare("notes") == 0) {
		displayNotes();
	}
}

void EraseObserver::displayTrack()
{
    string trackName = "";
    if(eraseGui->track == -1) {
        trackName = "ALL";
    } else {
        trackName = sequence->getTrack(eraseGui->track).lock()->getActualName();
    }
    trackField.lock()->setTextPadded(eraseGui->track + 1, " ");
    trackNameLabel.lock()->setText("-" + trackName);
}

void EraseObserver::displayTime()
{
	time0Field.lock()->setTextPadded(mpc::sequencer::SeqUtil::getBarFromTick(sequence, swGui->getTime0()) + 1, "0");
	time1Field.lock()->setTextPadded(mpc::sequencer::SeqUtil::getBeat(sequence, swGui->getTime0()) + 1, "0");
	time2Field.lock()->setTextPadded(mpc::sequencer::SeqUtil::getClockNumber(sequence, swGui->getTime0()), "0");
	time3Field.lock()->setTextPadded(mpc::sequencer::SeqUtil::getBarFromTick(sequence, swGui->getTime1()) + 1, "0");
	time4Field.lock()->setTextPadded(mpc::sequencer::SeqUtil::getBeat(sequence, swGui->getTime1()) + 1, "0");
	time5Field.lock()->setTextPadded(mpc::sequencer::SeqUtil::getClockNumber(sequence, swGui->getTime1()), "0");
}

void EraseObserver::displayErase()
{
    eraseField.lock()->setText(eraseNames[eraseGui->erase]);
}

void EraseObserver::displayType()
{
	typeField.lock()->Hide(eraseGui->getErase() < 0);
	if (eraseGui->erase > 0)
		typeField.lock()->setText(typeNames[eraseGui->type]);
}

void EraseObserver::displayNotes()
{
	auto lSampler = sampler.lock();
    if(eraseGui->erase != 0 && ((eraseGui->erase == 1 && eraseGui->type != 0) || (eraseGui->erase == 2 && eraseGui->type != 0))) {
	    notes0Field.lock()->Hide(true);
        notes0Label.lock()->Hide(true);
        notes1Field.lock()->Hide(true);
        notes1Label.lock()->Hide(true);
	    return;
    }
	notes0Field.lock()->Hide(false);
    notes0Label.lock()->Hide(false);
    notes1Field.lock()->Hide(bus != 0);
    notes1Label.lock()->Hide(bus != 0);
	if(bus > 0) {
        notes0Field.lock()->setSize(6 * 6 * 2 + 4, 18);
        if(swGui->getDrumNote() != 34) {
			notes0Field.lock()->setText(to_string(swGui->getDrumNote()) + "/" + lSampler->getPadName(program.lock()->getPadNumberFromNote(swGui->getDrumNote())));
        } else {
            notes0Field.lock()->setText("ALL");
        }
    } else {
        notes0Field.lock()->setSize(8 * 6 * 2, 18);
        //notes0Field.lock()->setText((moduru::lang::StrUtil::padLeft(to_string(swGui->getMidiNote0()), " ", 3) + mpc::ui::Uis::noteNames[swGui->getMidiNote0()]) + u8"\u00D4");
        //notes1Field.lock()->setText((moduru::lang::StrUtil::padLeft(to_string(swGui->getMidiNote1()), " ", 3) + mpc::ui::Uis::noteNames[swGui->getMidiNote1()]) + u8"\u00D4");
    }
}

EraseObserver::~EraseObserver() {
}
