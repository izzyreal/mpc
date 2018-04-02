#include <ui/sequencer/AssignObserver.hpp>

#include <Mpc.hpp>
////#include <lcdgui/LayeredScreen.hpp>
#include <lcdgui/Field.hpp>
#include <ui/sequencer/window/Assign16LevelsObserver.hpp>
#include <sampler/NoteParameters.hpp>
#include <sampler/Program.hpp>
#include <sampler/Sampler.hpp>
#include <sampler/PgmSlider.hpp>
#include <sequencer/Sequence.hpp>
#include <sequencer/Track.hpp>
#include <sequencer/Sequencer.hpp>

using namespace mpc::ui::sequencer;
using namespace std;

AssignObserver::AssignObserver(mpc::Mpc* mpc) 
{
	auto sequencer = mpc->getSequencer().lock();
	auto seq = sequencer->getActiveSequence().lock();
	sampler = mpc->getSampler();
	auto lSampler = sampler.lock();
	program = lSampler->getProgram(lSampler->getDrumBusProgramNumber(seq->getTrack(sequencer->getActiveTrackIndex()).lock()->getBusNumber()));
	slider = program.lock()->getSlider();
	slider->deleteObservers();
	slider->addObserver(this);
	auto ls = mpc->getLayeredScreen().lock();
	assignNoteField = ls->lookupField("assignnote");
	parameterField = ls->lookupField("parameter");
	highRangeField = ls->lookupField("highrange");
	lowRangeField = ls->lookupField("lowrange");
	assignNvField = ls->lookupField("assignnv");
	displayAssignNote();
	displayParameter();
	displayHighRange();
	displayLowRange();
	displayAssignNv();
}

void AssignObserver::displayAssignNote()
{
	auto nn = slider->getNote();
    if (nn == 34) {
        assignNoteField.lock()->setText("OFF");
		return;
    }
	auto lProgram = program.lock();
	auto lSampler = sampler.lock();
    string padName = lSampler->getPadName(lProgram->getPadNumberFromNote(nn));
    auto sn = lProgram->getNoteParameters(nn)->getSndNumber();
    auto soundName = sn == -1 ? "OFF" : lSampler->getSoundName(sn);
    assignNoteField.lock()->setText(to_string(nn) + "/" + padName + "-" + soundName);
}

void AssignObserver::displayParameter()
{
    parameterField.lock()->setText(mpc::ui::sequencer::window::Assign16LevelsObserver::TYPE_NAMES[slider->getParameter()]);
}

void AssignObserver::displayHighRange()
{
    auto hr = 0;
    switch (slider->getParameter()) {
    case 0:
        hr = slider->getTuneHighRange();
        break;
    case 1:
        hr = slider->getDecayHighRange();
        break;
    case 2:
        hr = slider->getAttackHighRange();
        break;
    case 3:
        hr = slider->getFilterHighRange();
        break;
    }
    highRangeField.lock()->setTextPadded(hr, " ");
}

void AssignObserver::displayLowRange()
{
    auto lr = 0;
    switch (slider->getParameter()) {
    case 0:
        lr = slider->getTuneLowRange();
        break;
    case 1:
        lr = slider->getDecayLowRange();
        break;
    case 2:
        lr = slider->getAttackLowRange();
        break;
    case 3:
        lr = slider->getFilterLowRange();
        break;
    }

    lowRangeField.lock()->setTextPadded(lr, " ");
}

void AssignObserver::displayAssignNv()
{
	auto assignNvString = slider->getControlChange() == 0 ? "OFF" : to_string(slider->getControlChange());
    assignNvField.lock()->setTextPadded(assignNvString, " ");
}

void AssignObserver::update(moduru::observer::Observable* o, boost::any arg)
{
	string s = boost::any_cast<string>(arg);
	if (s.compare("assignnote") == 0) {
		displayAssignNote();
	}
	else if (s.compare("parameter") == 0) {
		displayParameter();
		displayHighRange();
		displayLowRange();
	}
	else if (s.compare("highrange") == 0) {
		displayHighRange();
	}
	else if (s.compare("lowrange") == 0) {
		displayLowRange();
	}
	else if (s.compare("controlchange") == 0) {
		displayAssignNv();
	}
}

AssignObserver::~AssignObserver() {
	slider->deleteObserver(this);
}
