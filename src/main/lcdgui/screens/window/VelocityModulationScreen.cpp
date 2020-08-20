#include "VelocityModulationScreen.hpp"

#include <sampler/NoteParameters.hpp>
#include <controls/BaseSamplerControls.hpp>

using namespace mpc::lcdgui::screens::window;
using namespace mpc::controls;
using namespace moduru::lang;
using namespace std;

VelocityModulationScreen::VelocityModulationScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "velocity-modulation", layerIndex)
{
	baseControls = make_shared<BaseSamplerControls>(mpc);
}

void VelocityModulationScreen::open()
{
	init();
	displayNote();
	displayVeloAttack();
	displayVeloStart();
	displayVeloLevel();
	displayVelo();

	mpc.addObserver(this); // Subscribe to "padandnote" messages
}

void VelocityModulationScreen::close()
{
	mpc.deleteObserver(this);
}

void VelocityModulationScreen::turnWheel(int i)
{
	init();

	auto lastNp = sampler.lock()->getLastNp(program.lock().get());

	if (param.compare("veloattack") == 0)
	{
		lastNp->setVelocityToAttack(lastNp->getVelocityToAttack() + i);
		displayVeloAttack();
	}
	else if (param.compare("velostart") == 0)
	{
		lastNp->setVelocityToStart(lastNp->getVelocityToStart() + i);
		displayVeloStart();
	}
	else if (param.compare("velolevel") == 0)
	{
		lastNp->setVeloToLevel(lastNp->getVeloToLevel() + i);
		displayVeloLevel();
	}
	else if (param.compare("note") == 0)
	{
		mpc.setPadAndNote(mpc.getPad(), mpc.getNote() + i);
		displayNote();
	}
}

void VelocityModulationScreen::displayNote()
{
	auto noteParameters = sampler.lock()->getLastNp(program.lock().get());
	auto soundIndex = noteParameters->getSndNumber();
	auto padIndex = program.lock()->getPadIndexFromNote(noteParameters->getNumber());
	auto padName = padIndex != -1 ? sampler.lock()->getPadName(padIndex) : "OFF";
	auto sampleName = soundIndex != -1 ? sampler.lock()->getSoundName(soundIndex) : "OFF";
	string stereo = noteParameters->getStereoMixerChannel().lock()->isStereo() && soundIndex != -1 ? "(ST)" : "";
	findField("note").lock()->setText(to_string(noteParameters->getNumber()) + "/" + padName + "-" + StrUtil::padRight(sampleName, " ", 16) + stereo);
}

void VelocityModulationScreen::displayVelo()
{
	// Currently unimplemented
}

void VelocityModulationScreen::displayVeloAttack()
{
	findField("veloattack").lock()->setTextPadded(sampler.lock()->getLastNp(program.lock().get())->getVelocityToAttack(), " ");
}

void VelocityModulationScreen::displayVeloStart()
{
	auto lProgram = program.lock();
	findField("velostart").lock()->setTextPadded(sampler.lock()->getLastNp(program.lock().get())->getVelocityToStart(), " ");
}

void VelocityModulationScreen::displayVeloLevel()
{
	auto lProgram = program.lock();
	findField("velolevel").lock()->setTextPadded(sampler.lock()->getLastNp(program.lock().get())->getVeloToLevel(), " ");
}

void VelocityModulationScreen::update(moduru::observer::Observable* observable, nonstd::any message)
{
	auto msg = nonstd::any_cast<string>(message);

	if (msg.compare("padandnote") == 0)
	{
		displayNote();
		displayVeloAttack();
		displayVeloStart();
		displayVeloLevel();
	}
}
