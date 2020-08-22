#include "VeloPitchScreen.hpp"

#include <sampler/NoteParameters.hpp>
#include <controls/BaseSamplerControls.hpp>

using namespace mpc::lcdgui::screens::window;
using namespace mpc::controls;
using namespace moduru::lang;
using namespace std;

VeloPitchScreen::VeloPitchScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "velo-pitch", layerIndex)
{
	baseControls = make_shared<BaseSamplerControls>(mpc);
}

void VeloPitchScreen::open()
{
	init();

	displayNote();
	displayTune();
	displayVeloPitch();
	displayVelo();

	mpc.addObserver(this); // Subscribe to "padandnote" messages
}

void VeloPitchScreen::close()
{
	mpc.deleteObserver(this);
}

void VeloPitchScreen::turnWheel(int i)
{
	init();

	auto lastNp = sampler.lock()->getLastNp(program.lock().get());

	if (param.compare("tune") == 0)
	{
		lastNp->setTune(lastNp->getTune() + i);
		displayTune();
	}
	else if (param.compare("velo-pitch") == 0)
	{
		lastNp->setVelocityToPitch(lastNp->getVelocityToPitch() + i);
		displayVeloPitch();
	}
	else if (param.compare("note") == 0)
	{
		mpc.setPadAndNote(mpc.getPad(), mpc.getNote() + i);
		// We could call all display methods here, but we instead rely on the "padandnote" message
	}
}

void VeloPitchScreen::displayTune()
{
	auto value = sampler.lock()->getLastNp(program.lock().get())->getTune();
	string prefix = value < 0 ? "-" : " ";
	findField("tune").lock()->setText(prefix + StrUtil::padLeft(to_string(abs(value)), " ", 3));
}

void VeloPitchScreen::displayVeloPitch()
{
	auto value = sampler.lock()->getLastNp(program.lock().get())->getVelocityToPitch();
	string prefix = value < 0 ? "-" : " ";
	findField("velo-pitch").lock()->setText(prefix + StrUtil::padLeft(to_string(abs(value)), " ", 3));
}


void VeloPitchScreen::displayVelo()
{
	findField("velo").lock()->setText("127");
}

void VeloPitchScreen::update(moduru::observer::Observable* observable, nonstd::any message)
{
	auto msg = nonstd::any_cast<string>(message);

	if (msg.compare("padandnote") == 0)
	{
		displayNote();
		displayTune();
		displayVeloPitch();
	}
}

void VeloPitchScreen::displayNote()
{
	auto noteParameters = sampler.lock()->getLastNp(program.lock().get());
	auto soundIndex = noteParameters->getSndNumber();
	auto padIndex = program.lock()->getPadIndexFromNote(noteParameters->getNumber());
	auto padName = padIndex != -1 ? sampler.lock()->getPadName(padIndex) : "OFF";
	auto sampleName = soundIndex != -1 ? sampler.lock()->getSoundName(soundIndex) : "OFF";
	string stereo = noteParameters->getStereoMixerChannel().lock()->isStereo() && soundIndex != -1 ? "(ST)" : "";
	findField("note").lock()->setText(to_string(noteParameters->getNumber()) + "/" + padName + "-" + StrUtil::padRight(sampleName, " ", 16) + stereo);
}
