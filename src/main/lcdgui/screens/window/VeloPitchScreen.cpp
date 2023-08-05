#include "VeloPitchScreen.hpp"

#include <sampler/NoteParameters.hpp>

using namespace mpc::lcdgui::screens::window;
using namespace mpc::controls;

VeloPitchScreen::VeloPitchScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "velo-pitch", layerIndex)
{
}

void VeloPitchScreen::open()
{
	init();

	displayNote();
	displayTune();
	displayVeloPitch();
	displayVelo();

	mpc.addObserver(this); // Subscribe to "note" messages
}

void VeloPitchScreen::close()
{
	mpc.deleteObserver(this);
}

void VeloPitchScreen::turnWheel(int i)
{
	init();

	auto lastNp = sampler->getLastNp(program.get());

	if (param == "tune")
	{
		lastNp->setTune(lastNp->getTune() + i);
		displayTune();
	}
	else if (param == "velo-pitch")
	{
		lastNp->setVelocityToPitch(lastNp->getVelocityToPitch() + i);
		displayVeloPitch();
	}
	else if (param == "note")
	{
		mpc.setNote(mpc.getNote() + i);
		// We could call all display methods here, but we instead rely on the "note" message
	}
}

void VeloPitchScreen::displayTune()
{
	auto value = sampler->getLastNp(program.get())->getTune();
    std::string prefix = value < 0 ? "-" : " ";
	findField("tune")->setText(prefix + StrUtil::padLeft(std::to_string(abs(value)), " ", 3));
}

void VeloPitchScreen::displayVeloPitch()
{
	auto value = sampler->getLastNp(program.get())->getVelocityToPitch();
    std::string prefix = value < 0 ? "-" : " ";
	findField("velo-pitch")->setText(prefix + StrUtil::padLeft(std::to_string(abs(value)), " ", 3));
}


void VeloPitchScreen::displayVelo()
{
	findField("velo")->setText("127");
}

void VeloPitchScreen::update(moduru::observer::Observable* observable, nonstd::any message)
{
	auto msg = nonstd::any_cast<std::string>(message);

	if (msg == "note")
	{
		displayNote();
		displayTune();
		displayVeloPitch();
	}
}

void VeloPitchScreen::displayNote()
{
	auto noteParameters = sampler->getLastNp(program.get());
	auto soundIndex = noteParameters->getSoundIndex();
	auto padIndex = program->getPadIndexFromNote(noteParameters->getNumber());
	auto padName = sampler->getPadName(padIndex);
	auto sampleName = soundIndex != -1 ? sampler->getSoundName(soundIndex) : "OFF";
    std::string stereo = soundIndex != -1 && !sampler->getSound(soundIndex)->isMono() ? "(ST)" : "";
	findField("note")->setText(std::to_string(noteParameters->getNumber()) + "/" + padName + "-" + StrUtil::padRight(sampleName, " ", 16) + stereo);
}
