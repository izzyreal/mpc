#include "VeloEnvFilterScreen.hpp"

#include <sampler/NoteParameters.hpp>

using namespace mpc::lcdgui::screens::window;
using namespace moduru::lang;
using namespace std;

VeloEnvFilterScreen::VeloEnvFilterScreen(const int layerIndex) 
	: ScreenComponent("velo-env-filter", layerIndex)
{
}

void VeloEnvFilterScreen::open()
{
	addChild(make_shared<EnvGraph>());

	init();

	displayNote();
	displayAttack();
	displayDecay();
	displayAmount();
	displayVeloFreq();
	displayVelo();

	mpc.addObserver(this);
}

void VeloEnvFilterScreen::close()
{
	mpc.deleteObserver(this);
}

void VeloEnvFilterScreen::turnWheel(int i)
{
    init();

	auto lastNp = sampler.lock()->getLastNp(program.lock().get());

    if (param.compare("attack") == 0)
	{
		lastNp->setFilterAttack(lastNp->getFilterAttack() + i);
	}
	else if (param.compare("decay") == 0)
	{
		lastNp->setFilterDecay(lastNp->getFilterDecay() + i);
	}
	else if (param.compare("amount") == 0)
	{
		lastNp->setFilterEnvelopeAmount(lastNp->getFilterEnvelopeAmount() + i);
	}
	else if (param.compare("velofreq") == 0)
	{
		lastNp->setVelocityToFilterFrequency(lastNp->getVelocityToFilterFrequency() + i);
	}
	else if (param.compare("note") == 0)
	{
		mpc.setPadAndNote(mpc.getPad(), mpc.getNote() + i);
	}
}

void VeloEnvFilterScreen::update(moduru::observer::Observable* observable, nonstd::any message)
{
	auto msg = nonstd::any_cast<string>(message);

	if (msg.compare("padandnote") == 0)
	{
		displayNote();
		displayAttack();
		displayDecay();
		displayAmount();
		displayVeloFreq();
	}
}

void VeloEnvFilterScreen::displayNote()
{
	auto noteParameters = sampler.lock()->getLastNp(program.lock().get());
	auto soundIndex = noteParameters->getSndNumber();
	auto padIndex = program.lock()->getPadIndexFromNote(noteParameters->getNumber());
	auto pad = program.lock()->getPad(padIndex);
	auto padName = padIndex != -1 ? sampler.lock()->getPadName(padIndex) : "OFF";
	auto sampleName = soundIndex != -1 ? sampler.lock()->getSoundName(soundIndex) : "OFF";
	string stereo = pad->getStereoMixerChannel().lock()->isStereo() && soundIndex != -1 ? "(ST)" : "";
	findField("note").lock()->setText(to_string(noteParameters->getNumber()) + "/" + padName + "-" + StrUtil::padRight(sampleName, " ", 16) + stereo);
}

void VeloEnvFilterScreen::displayVelo()
{
	// Currently unimplemented
}

void VeloEnvFilterScreen::displayAttack()
{
	auto attack = sampler.lock()->getLastNp(program.lock().get())->getFilterAttack();
	auto decay = sampler.lock()->getLastNp(program.lock().get())->getFilterDecay();
	findField("attack").lock()->setTextPadded(attack, " ");
	findEnvGraph().lock()->setCoordinates(attack, decay, true);
}

void VeloEnvFilterScreen::displayDecay()
{
	auto attack = sampler.lock()->getLastNp(program.lock().get())->getFilterAttack();
	auto decay = sampler.lock()->getLastNp(program.lock().get())->getFilterDecay();
	findField("decay").lock()->setTextPadded(decay, " ");
	findEnvGraph().lock()->setCoordinates(attack, decay, true);
}

void VeloEnvFilterScreen::displayAmount()
{
	findField("amount").lock()->setTextPadded(sampler.lock()->getLastNp(program.lock().get())->getFilterEnvelopeAmount(), " ");
}

void VeloEnvFilterScreen::displayVeloFreq()
{
	findField("velofreq").lock()->setTextPadded(sampler.lock()->getLastNp(program.lock().get())->getVelocityToFilterFrequency(), " ");
}
