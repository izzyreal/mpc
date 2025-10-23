#include "VeloEnvFilterScreen.hpp"

using namespace mpc::lcdgui::screens::window;

VeloEnvFilterScreen::VeloEnvFilterScreen(mpc::Mpc& mpc, const int layerIndex) 
    : ScreenComponent(mpc, "velo-env-filter", layerIndex)
{
    addChildT<EnvGraph>(mpc);
}

void VeloEnvFilterScreen::open()
{
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

    auto program = getProgramOrThrow();
    auto lastNp = sampler->getLastNp(program.get());

    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName == "attack")
    {
        lastNp->setFilterAttack(lastNp->getFilterAttack() + i);
        displayAttack();
    }
    else if (focusedFieldName == "decay")
    {
        lastNp->setFilterDecay(lastNp->getFilterDecay() + i);
        displayDecay();
    }
    else if (focusedFieldName == "amount")
    {
        lastNp->setFilterEnvelopeAmount(lastNp->getFilterEnvelopeAmount() + i);
        displayAmount();
    }
    else if (focusedFieldName == "velofreq")
    {
        lastNp->setVelocityToFilterFrequency(lastNp->getVelocityToFilterFrequency() + i);
        displayVeloFreq();
    }
    else if (focusedFieldName == "note")
    {
        mpc.setNote(mpc.getNote() + i);
    }
    else if (focusedFieldName == "velo")
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
    auto program = getProgramOrThrow();
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
    auto program = getProgramOrThrow();
    auto attack = sampler->getLastNp(program.get())->getFilterAttack();
    auto decay = sampler->getLastNp(program.get())->getFilterDecay();
    findField("attack")->setTextPadded(attack, " ");
    findEnvGraph()->setCoordinates(attack, decay, true);
}

void VeloEnvFilterScreen::displayDecay()
{
    auto program = getProgramOrThrow();
    auto attack = sampler->getLastNp(program.get())->getFilterAttack();
    auto decay = sampler->getLastNp(program.get())->getFilterDecay();
    findField("decay")->setTextPadded(decay, " ");
    findEnvGraph()->setCoordinates(attack, decay, true);
}

void VeloEnvFilterScreen::displayAmount()
{
    auto program = getProgramOrThrow();
    findField("amount")->setTextPadded(sampler->getLastNp(program.get())->getFilterEnvelopeAmount(), " ");
}

void VeloEnvFilterScreen::displayVeloFreq()
{
    auto program = getProgramOrThrow();
    findField("velofreq")->setTextPadded(sampler->getLastNp(program.get())->getVelocityToFilterFrequency(), " ");
}

void VeloEnvFilterScreen::setVelo(int i)
{
    velo = std::clamp(i, 1, 127);
    displayVelo();
}
