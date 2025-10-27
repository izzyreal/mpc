#include "VeloEnvFilterScreen.hpp"
#include "controller/ClientEventController.hpp"

using namespace mpc::lcdgui::screens::window;

VeloEnvFilterScreen::VeloEnvFilterScreen(mpc::Mpc &mpc, const int layerIndex)
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

    mpc.clientEventController->addObserver(this);
}

void VeloEnvFilterScreen::close()
{
    mpc.clientEventController->deleteObserver(this);
}

void VeloEnvFilterScreen::turnWheel(int i)
{
    auto program = getProgramOrThrow();
    auto selectedNoteParameters = program->getNoteParameters(mpc.clientEventController->getSelectedNote());

    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName == "attack")
    {
        selectedNoteParameters->setFilterAttack(selectedNoteParameters->getFilterAttack() + i);
        displayAttack();
    }
    else if (focusedFieldName == "decay")
    {
        selectedNoteParameters->setFilterDecay(selectedNoteParameters->getFilterDecay() + i);
        displayDecay();
    }
    else if (focusedFieldName == "amount")
    {
        selectedNoteParameters->setFilterEnvelopeAmount(selectedNoteParameters->getFilterEnvelopeAmount() + i);
        displayAmount();
    }
    else if (focusedFieldName == "velofreq")
    {
        selectedNoteParameters->setVelocityToFilterFrequency(selectedNoteParameters->getVelocityToFilterFrequency() + i);
        displayVeloFreq();
    }
    else if (focusedFieldName == "note")
    {
        mpc.clientEventController->setSelectedNote(mpc.clientEventController->getSelectedNote() + i);
    }
    else if (focusedFieldName == "velo")
    {
        setVelo(velo + i);
    }
}

void VeloEnvFilterScreen::update(Observable *observable, Message message)
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
    auto selectedNoteParameters = program->getNoteParameters(mpc.clientEventController->getSelectedNote());
    auto soundIndex = selectedNoteParameters->getSoundIndex();
    auto padIndex = program->getPadIndexFromNote(selectedNoteParameters->getNumber());
    auto padName = sampler->getPadName(padIndex);
    auto sampleName = soundIndex != -1 ? sampler->getSoundName(soundIndex) : "OFF";
    std::string stereo = soundIndex != -1 && !sampler->getSound(soundIndex)->isMono() ? "(ST)" : "";
    findField("note")->setText(std::to_string(selectedNoteParameters->getNumber()) + "/" + padName + "-" + StrUtil::padRight(sampleName, " ", 16) + stereo);
}

void VeloEnvFilterScreen::displayVelo()
{
    findField("velo")->setTextPadded(velo, " ");
}

void VeloEnvFilterScreen::displayAttack()
{
    auto program = getProgramOrThrow();
    auto selectedNoteParameters = program->getNoteParameters(mpc.clientEventController->getSelectedNote());
    auto attack = selectedNoteParameters->getFilterAttack();
    auto decay = selectedNoteParameters->getFilterDecay();
    findField("attack")->setTextPadded(attack, " ");
    findEnvGraph()->setCoordinates(attack, decay, true);
}

void VeloEnvFilterScreen::displayDecay()
{
    auto program = getProgramOrThrow();
    auto selectedNoteParameters = program->getNoteParameters(mpc.clientEventController->getSelectedNote());
    auto attack = selectedNoteParameters->getFilterAttack();
    auto decay = selectedNoteParameters->getFilterDecay();
    findField("decay")->setTextPadded(decay, " ");
    findEnvGraph()->setCoordinates(attack, decay, true);
}

void VeloEnvFilterScreen::displayAmount()
{
    auto program = getProgramOrThrow();
    auto selectedNoteParameters = program->getNoteParameters(mpc.clientEventController->getSelectedNote());
    findField("amount")->setTextPadded(selectedNoteParameters->getFilterEnvelopeAmount(), " ");
}

void VeloEnvFilterScreen::displayVeloFreq()
{
    auto program = getProgramOrThrow();
    auto selectedNoteParameters = program->getNoteParameters(mpc.clientEventController->getSelectedNote());
    findField("velofreq")->setTextPadded(selectedNoteParameters->getVelocityToFilterFrequency(), " ");
}

void VeloEnvFilterScreen::setVelo(int i)
{
    velo = std::clamp(i, 1, 127);
    displayVelo();
}

