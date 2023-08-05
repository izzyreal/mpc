#include "MuteAssignScreen.hpp"

using namespace mpc::lcdgui::screens::window;
using namespace mpc::controls;

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
	auto lastNoteParameters = sampler->getLastNp(program.get());

	if (param == "note")
	{
		mpc.setNote(mpc.getNote() + i);
	}
	else if (param == "note0")
	{
		lastNoteParameters->setMuteAssignA(lastNoteParameters->getMuteAssignA() + i);
		displayNote0();
	}
	else if (param == "note1")
	{
		lastNoteParameters->setMuteAssignB(lastNoteParameters->getMuteAssignB() + i);
		displayNote1();
	}
}

void MuteAssignScreen::displayNote()
{
	auto note = sampler->getLastNp(program.get())->getNumber();
	auto pad = program->getPadIndexFromNote(note);
    std::string soundName = "OFF";

    auto padName = sampler->getPadName(pad);
	auto sound = program->getNoteParameters(note)->getSoundIndex();

    if (sound != -1)
    {
        soundName = sampler->getSoundName(sound);
    }

	findField("note")->setText(std::to_string(note) + "/" + padName + "-" + soundName);
}

void MuteAssignScreen::displayNote0()
{

	auto note0 = sampler->getLastNp(program.get())->getMuteAssignA();

	if (note0 == 34)
	{
		findField("note0")->setText("--");
		return;
	}

	auto pad = program->getPadIndexFromNote(note0);
    std::string soundName = "OFF";
	auto sound = program->getNoteParameters(note0)->getSoundIndex();
	
	if (sound != -1)
	{
		soundName = sampler->getSoundName(sound);
	}

	findField("note0")->setText(std::to_string(note0) + "/" + sampler->getPadName(pad) + "-" + soundName);
}

void MuteAssignScreen::displayNote1()
{
	auto note1 = sampler->getLastNp(program.get())->getMuteAssignB();

	if (note1 == 34)
	{
		findField("note1")->setText("--");
		return;
	}
	
	auto pad = program->getPadIndexFromNote(note1);
    std::string soundName = "OFF";
	auto sound = program->getNoteParameters(note1)->getSoundIndex();

	if (sound != -1)
	{
		soundName = sampler->getSoundName(sound);
	}

	findField("note1")->setText(std::to_string(note1) + "/" + sampler->getPadName(pad) + "-" + soundName);
}

void MuteAssignScreen::update(moduru::observer::Observable* o, nonstd::any arg)
{
	auto s = nonstd::any_cast<std::string>(arg);

	if (s == "note")
	{
		displayNote();
		displayNote0();
		displayNote1();
	}
}
