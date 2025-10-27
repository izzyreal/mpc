#include "VelocityModulationScreen.hpp"
#include "controller/ClientEventController.hpp"

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
    auto program = getProgramOrThrow();
    auto selectedNoteParameters = program->getNoteParameters(
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
    auto program = getProgramOrThrow();
    auto selectedNoteParameters = program->getNoteParameters(
        mpc.clientEventController->getSelectedNote());
    auto soundIndex = selectedNoteParameters->getSoundIndex();
    auto padIndex =
        program->getPadIndexFromNote(selectedNoteParameters->getNumber());
    auto padName = sampler->getPadName(padIndex);
    auto sampleName =
        soundIndex != -1 ? sampler->getSoundName(soundIndex) : "OFF";
    std::string stereo =
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
    auto program = getProgramOrThrow();
    auto selectedNoteParameters = program->getNoteParameters(
        mpc.clientEventController->getSelectedNote());
    findField("veloattack")
        ->setTextPadded(selectedNoteParameters->getVelocityToAttack(), " ");
}

void VelocityModulationScreen::displayVeloStart()
{
    auto program = getProgramOrThrow();
    auto selectedNoteParameters = program->getNoteParameters(
        mpc.clientEventController->getSelectedNote());
    findField("velostart")
        ->setTextPadded(selectedNoteParameters->getVelocityToStart(), " ");
}

void VelocityModulationScreen::displayVeloLevel()
{
    auto program = getProgramOrThrow();
    auto selectedNoteParameters = program->getNoteParameters(
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
