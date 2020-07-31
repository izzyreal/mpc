#include "VeloEnvFilterScreen.hpp"

#include <sampler/NoteParameters.hpp>

using namespace mpc::lcdgui::screens::window;
using namespace moduru::lang;
using namespace std;

VeloEnvFilterScreen::VeloEnvFilterScreen(mpc::Mpc& mpc, const int layerIndex) 
	: ScreenComponent(mpc, "velo-env-filter", layerIndex)
{
	addChild(make_shared<EnvGraph>(mpc));
}

void VeloEnvFilterScreen::open()
{
	init();
	velo = 127;
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
		displayAttack();
	}
	else if (param.compare("decay") == 0)
	{
		lastNp->setFilterDecay(lastNp->getFilterDecay() + i);
		displayDecay();
	}
	else if (param.compare("amount") == 0)
	{
		lastNp->setFilterEnvelopeAmount(lastNp->getFilterEnvelopeAmount() + i);
		displayAmount();
	}
	else if (param.compare("velofreq") == 0)
	{
		lastNp->setVelocityToFilterFrequency(lastNp->getVelocityToFilterFrequency() + i);
		displayVeloFreq();
	}
	else if (param.compare("note") == 0)
	{
		mpc.setPadAndNote(mpc.getPad(), mpc.getNote() + i);
	}
	else if (param.compare("velo") == 0)
	{
		setVelo(velo + i);
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
	findField("velo").lock()->setTextPadded(velo, " ");
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

void VeloEnvFilterScreen::setVelo(int i)
{
	if (i < 1 || i > 127)
	{
		return;
	}

	velo = i;

	displayVelo();
}
