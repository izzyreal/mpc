#include "MuteAssignScreen.hpp"

#include <sampler/NoteParameters.hpp>

using namespace mpc::lcdgui::screens::window;
using namespace mpc::controls;
using namespace moduru::lang;
using namespace std;

MuteAssignScreen::MuteAssignScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "mute-assign", layerIndex)
{
}

void MuteAssignScreen::open()
{
	init();
	displayNote();
	displayNote0();
	displayNote1();
	mpc.addObserver(this); // Subscribe to "note" messages
}

void MuteAssignScreen::close()
{
	mpc.deleteObserver(this);
}

void MuteAssignScreen::turnWheel(int i)
{
    init();
	auto lastNoteParameters = sampler.lock()->getLastNp(program.lock().get());

	if (param.compare("note") == 0)
	{
		mpc.setNote(mpc.getNote() + i);
	}
	else if (param.compare("note0") == 0)
	{
		lastNoteParameters->setMuteAssignA(lastNoteParameters->getMuteAssignA() + i);
		displayNote0();
	}
	else if (param.compare("note1") == 0)
	{
		lastNoteParameters->setMuteAssignB(lastNoteParameters->getMuteAssignB() + i);
		displayNote1();
	}
}

void MuteAssignScreen::displayNote()
{
	auto note = sampler.lock()->getLastNp(program.lock().get())->getNumber();
	auto pad = program.lock()->getPadIndexFromNote(note);
	string soundName = "OFF";

    auto padName = sampler.lock()->getPadName(pad);
	auto sound = program.lock()->getNoteParameters(note)->getSoundIndex();

    if (sound != -1)
    {
        soundName = sampler.lock()->getSoundName(sound);
    }

	findField("note").lock()->setText(to_string(note) + "/" + padName + "-" + soundName);
}

void MuteAssignScreen::displayNote0()
{

	auto note0 = sampler.lock()->getLastNp(program.lock().get())->getMuteAssignA();

	if (note0 == 34)
	{
		findField("note0").lock()->setText("--");
		return;
	}

	auto pad = program.lock()->getPadIndexFromNote(note0);
	string soundName = "OFF";
	auto sound = program.lock()->getNoteParameters(note0)->getSoundIndex();
	
	if (sound != -1)
	{
		soundName = sampler.lock()->getSoundName(sound);
	}

	findField("note0").lock()->setText(to_string(note0) + "/" + sampler.lock()->getPadName(pad) + "-" + soundName);
}

void MuteAssignScreen::displayNote1()
{
	auto note1 = sampler.lock()->getLastNp(program.lock().get())->getMuteAssignB();

	if (note1 == 34)
	{
		findField("note1").lock()->setText("--");
		return;
	}
	
	auto pad = program.lock()->getPadIndexFromNote(note1);
	string soundName = "OFF";
	auto sound = program.lock()->getNoteParameters(note1)->getSoundIndex();

	if (sound != -1)
	{
		soundName = sampler.lock()->getSoundName(sound);
	}

	findField("note1").lock()->setText(to_string(note1) + "/" + sampler.lock()->getPadName(pad) + "-" + soundName);
}

void MuteAssignScreen::update(moduru::observer::Observable* o, nonstd::any arg)
{
	string s = nonstd::any_cast<string>(arg);

	if (s.compare("note") == 0)
	{
		displayNote();
		displayNote0();
		displayNote1();
	}
}
