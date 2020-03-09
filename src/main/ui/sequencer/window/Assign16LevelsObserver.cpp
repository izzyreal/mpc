#include <ui/sequencer/window/Assign16LevelsObserver.hpp>

#include <Mpc.hpp>
#include <ui/Uis.hpp>
#include <lcdgui/LayeredScreen.hpp>
#include <lcdgui/Field.hpp>
#include <lcdgui/Label.hpp>
#include <ui/sequencer/SequencerGui.hpp>
#include <sampler/NoteParameters.hpp>
#include <sampler/Program.hpp>
#include <sampler/Sampler.hpp>
#include <sequencer/Sequence.hpp>
#include <sequencer/Track.hpp>
#include <sequencer/Sequencer.hpp>

#include <lang/StrUtil.hpp>

using namespace mpc::ui::sequencer::window;
using namespace std;

Assign16LevelsObserver::Assign16LevelsObserver(mpc::Mpc* mpc) 
{
	this->mpc = mpc;
	sGui = mpc->getUis().lock()->getSequencerGui();
	sGui->addObserver(this);
	auto ls = mpc->getLayeredScreen().lock();
	noteField = ls->lookupField("note");
	paramField = ls->lookupField("param");
	typeField = ls->lookupField("type");
	typeLabel = ls->lookupLabel("type");
	originalKeyPadField = ls->lookupField("originalkeypad");
	originalKeyPadLabel = ls->lookupLabel("originalkeypad");
	displayNote();
	displayParameter();
	if (sGui->getParameter() == 1) {
		displayType();
		displayOriginalKeyPad();
	}
}

vector<string> Assign16LevelsObserver::PARAM_NAMES { "VELOCITY", "NOTE VAR" };
vector<string> Assign16LevelsObserver::TYPE_NAMES { "TUNING", "DECAY", "ATTACK", "FILTER" };

void Assign16LevelsObserver::displayNote()
{
    auto seq = mpc->getSequencer().lock();
	auto sampler = mpc->getSampler().lock();
    auto nn = sGui->getNote();
    auto track = seq->getActiveTrackIndex();
    auto pgmNumber = sampler->getDrumBusProgramNumber(seq->getActiveSequence().lock()->getTrack(track).lock()->getBusNumber());
	auto program = sampler->getProgram(pgmNumber).lock();
    auto pn = program->getPadNumberFromNote(nn);
    auto sn = program->getNoteParameters(nn)->getSndNumber();
    auto soundName = sn == -1 ? "OFF" : sampler->getSoundName(sn);
    noteField.lock()->setText(to_string(nn) + "/" + sampler->getPadName(pn) + "-" + soundName);
}

void Assign16LevelsObserver::displayParameter()
{
    paramField.lock()->setText(PARAM_NAMES[sGui->getParameter()]);
    typeField.lock()->Hide(sGui->getParameter() != 1);
	typeLabel.lock()->Hide(sGui->getParameter() != 1);
    originalKeyPadField.lock()->Hide(!(sGui->getParameter() == 1 && sGui->getType() == 0));
    originalKeyPadLabel.lock()->Hide(!(sGui->getParameter() == 1 && sGui->getType() == 0));
}

void Assign16LevelsObserver::displayType()
{
    typeField.lock()->setText(TYPE_NAMES[sGui->getType()]);
    originalKeyPadField.lock()->Hide(sGui->getType() != 0);
    originalKeyPadLabel.lock()->Hide(sGui->getType() != 0);
}

void Assign16LevelsObserver::displayOriginalKeyPad()
{
    originalKeyPadField.lock()->setTextPadded(sGui->getOriginalKeyPad() + 1, " ");
}

void Assign16LevelsObserver::update(moduru::observer::Observable* o, nonstd::any arg)
{
	string s = nonstd::any_cast<string>(arg);
	if (s.compare("note") == 0) {
		displayNote();
	}
	else if (s.compare("parameter") == 0) {
		displayParameter();
		if (sGui->getParameter() == 1) {
			displayType();
			displayOriginalKeyPad();
		}
	}
	else if (s.compare("type") == 0) {
		displayType();
	}
	else if (s.compare("originalkeypad") == 0) {
		displayOriginalKeyPad();
	}
}

Assign16LevelsObserver::~Assign16LevelsObserver() {
	sGui->deleteObserver(this);
}
