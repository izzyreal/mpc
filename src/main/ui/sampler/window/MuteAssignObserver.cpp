#include "MuteAssignObserver.hpp"

#include <Mpc.hpp>
#include <lcdgui/Field.hpp>
#include <ui/sampler/SamplerGui.hpp>
#include <sampler/NoteParameters.hpp>
#include <sampler/Program.hpp>
#include <sampler/Sampler.hpp>

using namespace mpc::ui::sampler::window;
using namespace std;

MuteAssignObserver::MuteAssignObserver() 
{
	auto uis = Mpc::instance().getUis().lock();
	samplerGui = uis->getSamplerGui();
	samplerGui->addObserver(this);
	
	sampler = Mpc::instance().getSampler();
	auto drum = uis->getSamplerGui()->getSelectedDrum();
	auto ls = Mpc::instance().getLayeredScreen().lock();
	noteField = ls->lookupField("note");
	note0Field = ls->lookupField("note0");
	note1Field = ls->lookupField("note1");
	auto lSampler = sampler.lock();
	program = dynamic_pointer_cast<mpc::sampler::Program>(lSampler->getProgram(lSampler->getDrumBusProgramNumber(uis->getSamplerGui()->getSelectedDrum() + 1)).lock());
	auto lProgram = program.lock();
	np = lSampler->getLastNp(lProgram.get());
	np->addObserver(this);
	displayNote();
	displayNote0();
	displayNote1();
}

void MuteAssignObserver::displayNote()
{
	auto lSampler = sampler.lock();
	auto lProgram = program.lock();
	auto note = lSampler->getLastNp(lProgram.get())->getNumber();
	auto pad = lProgram->getPadNumberFromNote(note);
	string soundName = "OFF";
	auto padName = pad == -1 ? "OFF" : lSampler->getPadName(pad);
	auto sound = lProgram->getNoteParameters(note)->getSndNumber();
	if (sound != -1) soundName = lSampler->getSoundName(sound);

	noteField.lock()->setText(to_string(note) + "/" + padName + "-" + soundName);
}

void MuteAssignObserver::displayNote0()
{
	auto lSampler = sampler.lock();
	auto lProgram = program.lock();
	auto note0 = lSampler->getLastNp(lProgram.get())->getMuteAssignA();
	if (note0 == 34) {
		note0Field.lock()->setText("--");
		return;
	}
	auto pad = lProgram->getPadNumberFromNote(note0);
	string soundName = "OFF";
	auto sound = lProgram->getNoteParameters(note0)->getSndNumber();
	if (sound != -1) soundName = lSampler->getSoundName(sound);

	note0Field.lock()->setText(to_string(note0) + "/" + lSampler->getPadName(pad) + "-" + soundName);
}

void MuteAssignObserver::displayNote1()
{
	auto lSampler = sampler.lock();
	auto lProgram = program.lock();
	auto note1 = lSampler->getLastNp(lProgram.get())->getMuteAssignB();
	if (note1 == 34) {
		note1Field.lock()->setText("--");
		return;
	}
	auto pad = lProgram->getPadNumberFromNote(note1);
	string soundName = "OFF";
	auto sound = lProgram->getNoteParameters(note1)->getSndNumber();
	if (sound != -1) soundName = lSampler->getSoundName(sound);

	note1Field.lock()->setText(to_string(note1) + "/" + lSampler->getPadName(pad) + "-" + soundName);
}

void MuteAssignObserver::update(moduru::observer::Observable* o, nonstd::any arg)
{
	auto lSampler = sampler.lock();
	auto lProgram = program.lock();
	np->deleteObserver(this);
	np = lSampler->getLastNp(lProgram.get());
	np->addObserver(this);

	string s = nonstd::any_cast<string>(arg);

	if (s.compare("padandnote") == 0) {
		displayNote();
		displayNote0();
		displayNote1();
	}
	else if (s.compare("muteassigna") == 0) {
		displayNote0();
	}
	else if (s.compare("muteassignb") == 0) {
		displayNote1();
	}
}

MuteAssignObserver::~MuteAssignObserver() {
	samplerGui->deleteObserver(this);
	np->deleteObserver(this);
}
