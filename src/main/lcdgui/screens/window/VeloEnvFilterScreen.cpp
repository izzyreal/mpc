#include "VeloEnvFilterScreen.hpp"

using namespace mpc::lcdgui::screens::window;
using namespace mpc::controls;

VeloEnvFilterScreen::VeloEnvFilterScreen(mpc::Mpc& mpc, const int layerIndex) 
	: ScreenComponent(mpc, "velo-env-filter", layerIndex)
{
	addChildT<EnvGraph>(mpc);
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

	auto lastNp = sampler->getLastNp(program.get());

    if (param == "attack")
	{
		lastNp->setFilterAttack(lastNp->getFilterAttack() + i);
		displayAttack();
	}
	else if (param == "decay")
	{
		lastNp->setFilterDecay(lastNp->getFilterDecay() + i);
		displayDecay();
	}
	else if (param == "amount")
	{
		lastNp->setFilterEnvelopeAmount(lastNp->getFilterEnvelopeAmount() + i);
		displayAmount();
	}
	else if (param == "velofreq")
	{
		lastNp->setVelocityToFilterFrequency(lastNp->getVelocityToFilterFrequency() + i);
		displayVeloFreq();
	}
	else if (param == "note")
	{
		mpc.setNote(mpc.getNote() + i);
	}
	else if (param == "velo")
	{
		setVelo(velo + i);
	}
}

void VeloEnvFilterScreen::update(Observable* observable, Message message)
{
	const auto msg = std::get<std::string>(message);

	if (msg == "note")
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
	auto noteParameters = sampler->getLastNp(program.get());
	auto soundIndex = noteParameters->getSoundIndex();
	auto padIndex = program->getPadIndexFromNote(noteParameters->getNumber());
	auto padName = sampler->getPadName(padIndex);
	auto sampleName = soundIndex != -1 ? sampler->getSoundName(soundIndex) : "OFF";
    std::string stereo = soundIndex != -1 && !sampler->getSound(soundIndex)->isMono() ? "(ST)" : "";
	findField("note")->setText(std::to_string(noteParameters->getNumber()) + "/" + padName + "-" + StrUtil::padRight(sampleName, " ", 16) + stereo);
}

void VeloEnvFilterScreen::displayVelo()
{
	findField("velo")->setTextPadded(velo, " ");
}

void VeloEnvFilterScreen::displayAttack()
{
	auto attack = sampler->getLastNp(program.get())->getFilterAttack();
	auto decay = sampler->getLastNp(program.get())->getFilterDecay();
	findField("attack")->setTextPadded(attack, " ");
	findEnvGraph()->setCoordinates(attack, decay, true);
}

void VeloEnvFilterScreen::displayDecay()
{
	auto attack = sampler->getLastNp(program.get())->getFilterAttack();
	auto decay = sampler->getLastNp(program.get())->getFilterDecay();
	findField("decay")->setTextPadded(decay, " ");
	findEnvGraph()->setCoordinates(attack, decay, true);
}

void VeloEnvFilterScreen::displayAmount()
{
	findField("amount")->setTextPadded(sampler->getLastNp(program.get())->getFilterEnvelopeAmount(), " ");
}

void VeloEnvFilterScreen::displayVeloFreq()
{
	findField("velofreq")->setTextPadded(sampler->getLastNp(program.get())->getVelocityToFilterFrequency(), " ");
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
