#include "VeloPitchScreen.hpp"

#include <sampler/NoteParameters.hpp>

using namespace mpc::lcdgui::screens::window;

VeloPitchScreen::VeloPitchScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "velo-pitch", layerIndex)
{
}

void VeloPitchScreen::open()
{

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

    auto program = getProgramOrThrow();
	auto lastNp = sampler->getLastNp(program.get());

    const auto focusedFieldName = getFocusedFieldNameOrThrow();

	if (focusedFieldName == "tune")
	{
		lastNp->setTune(lastNp->getTune() + i);
		displayTune();
	}
	else if (focusedFieldName == "velo-pitch")
	{
		lastNp->setVelocityToPitch(lastNp->getVelocityToPitch() + i);
		displayVeloPitch();
	}
	else if (focusedFieldName == "note")
	{
		mpc.setNote(mpc.getNote() + i);
		// We could call all display methods here, but we instead rely on the "note" message
	}
}

void VeloPitchScreen::displayTune()
{
    auto program = getProgramOrThrow();
	auto value = sampler->getLastNp(program.get())->getTune();
    std::string prefix = value < 0 ? "-" : " ";
	findField("tune")->setText(prefix + StrUtil::padLeft(std::to_string(abs(value)), " ", 3));
}

void VeloPitchScreen::displayVeloPitch()
{
    auto program = getProgramOrThrow();
	auto value = sampler->getLastNp(program.get())->getVelocityToPitch();
    std::string prefix = value < 0 ? "-" : " ";
	findField("velo-pitch")->setText(prefix + StrUtil::padLeft(std::to_string(abs(value)), " ", 3));
}


void VeloPitchScreen::displayVelo()
{
	findField("velo")->setText("127");
}

void VeloPitchScreen::update(Observable* observable, Message message)
{
	const auto msg = std::get<std::string>(message);

	if (msg == "note")
	{
		displayNote();
		displayTune();
		displayVeloPitch();
	}
}

void VeloPitchScreen::displayNote()
{
    auto program = getProgramOrThrow();
	auto noteParameters = sampler->getLastNp(program.get());
	auto soundIndex = noteParameters->getSoundIndex();
	auto padIndex = program->getPadIndexFromNote(noteParameters->getNumber());
	auto padName = sampler->getPadName(padIndex);
	auto sampleName = soundIndex != -1 ? sampler->getSoundName(soundIndex) : "OFF";
    std::string stereo = soundIndex != -1 && !sampler->getSound(soundIndex)->isMono() ? "(ST)" : "";
	findField("note")->setText(std::to_string(noteParameters->getNumber()) + "/" + padName + "-" + StrUtil::padRight(sampleName, " ", 16) + stereo);
}
