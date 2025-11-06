#include "VelocityModulationScreen.hpp"

#include "Mpc.hpp"
#include "StrUtil.hpp"
#include "controller/ClientEventController.hpp"
#include "sampler/Sampler.hpp"

using namespace mpc::lcdgui::screens::window;

VelocityModulationScreen::VelocityModulationScreen(mpc::Mpc &mpc,
                                                   const int layerIndex)
    : ScreenComponent(mpc, "velocity-modulation", layerIndex)
{
}

void VelocityModulationScreen::open()
{
    displayNote();
    displayVeloAttack();
    displayVeloStart();
    displayVeloLevel();
    displayVelo();

    mpc.clientEventController->addObserver(
        this); // Subscribe to "note" messages
}

void VelocityModulationScreen::close()
{
    mpc.clientEventController->deleteObserver(this);
}

void VelocityModulationScreen::turnWheel(int i)
{
    const auto program = getProgramOrThrow();
    const auto selectedNoteParameters = program->getNoteParameters(
        mpc.clientEventController->getSelectedNote());

    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName == "veloattack")
    {
        selectedNoteParameters->setVelocityToAttack(
            selectedNoteParameters->getVelocityToAttack() + i);
        displayVeloAttack();
    }
    else if (focusedFieldName == "velostart")
    {
        selectedNoteParameters->setVelocityToStart(
            selectedNoteParameters->getVelocityToStart() + i);
        displayVeloStart();
    }
    else if (focusedFieldName == "velolevel")
    {
        selectedNoteParameters->setVeloToLevel(
            selectedNoteParameters->getVeloToLevel() + i);
        displayVeloLevel();
    }
    else if (focusedFieldName == "note")
    {
        mpc.clientEventController->setSelectedNote(
            mpc.clientEventController->getSelectedNote() + i);
        displayNote();
    }
}

void VelocityModulationScreen::displayNote()
{
    const auto program = getProgramOrThrow();
    const auto selectedNoteParameters = program->getNoteParameters(
        mpc.clientEventController->getSelectedNote());
    const auto soundIndex = selectedNoteParameters->getSoundIndex();
    const auto padIndex =
        program->getPadIndexFromNote(selectedNoteParameters->getNumber());
    const auto padName = sampler->getPadName(padIndex);
    const auto sampleName =
        soundIndex != -1 ? sampler->getSoundName(soundIndex) : "OFF";
    const std::string stereo =
        soundIndex != -1 && !sampler->getSound(soundIndex)->isMono() ? "(ST)"
                                                                     : "";
    findField("note")->setText(
        std::to_string(selectedNoteParameters->getNumber()) + "/" + padName +
        "-" + StrUtil::padRight(sampleName, " ", 16) + stereo);
}

void VelocityModulationScreen::displayVelo()
{
    findField("velo")->setText("127"); // Unimplemented.
}

void VelocityModulationScreen::displayVeloAttack()
{
    const auto program = getProgramOrThrow();
    const auto selectedNoteParameters = program->getNoteParameters(
        mpc.clientEventController->getSelectedNote());
    findField("veloattack")
        ->setTextPadded(selectedNoteParameters->getVelocityToAttack(), " ");
}

void VelocityModulationScreen::displayVeloStart()
{
    const auto program = getProgramOrThrow();
    const auto selectedNoteParameters = program->getNoteParameters(
        mpc.clientEventController->getSelectedNote());
    findField("velostart")
        ->setTextPadded(selectedNoteParameters->getVelocityToStart(), " ");
}

void VelocityModulationScreen::displayVeloLevel()
{
    const auto program = getProgramOrThrow();
    const auto selectedNoteParameters = program->getNoteParameters(
        mpc.clientEventController->getSelectedNote());
    findField("velolevel")
        ->setTextPadded(selectedNoteParameters->getVeloToLevel(), " ");
}

void VelocityModulationScreen::update(Observable *observable, Message message)
{
    const auto msg = std::get<std::string>(message);

    if (msg == "note")
    {
        displayNote();
        displayVeloAttack();
        displayVeloStart();
        displayVeloLevel();
    }
}
