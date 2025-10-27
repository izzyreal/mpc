#include "VeloPitchScreen.hpp"

#include "controller/ClientEventController.hpp"
#include "sampler/NoteParameters.hpp"

using namespace mpc::lcdgui::screens::window;

VeloPitchScreen::VeloPitchScreen(mpc::Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "velo-pitch", layerIndex)
{
}

void VeloPitchScreen::open()
{
    displayNote();
    displayTune();
    displayVeloPitch();
    displayVelo();

    mpc.clientEventController->addObserver(
        this); // Subscribe to "note" messages
}

void VeloPitchScreen::close()
{
    mpc.clientEventController->deleteObserver(this);
}

void VeloPitchScreen::turnWheel(int i)
{
    auto program = getProgramOrThrow();
    auto selectedNoteParameters = program->getNoteParameters(
        mpc.clientEventController->getSelectedNote());

    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName == "tune")
    {
        selectedNoteParameters->setTune(selectedNoteParameters->getTune() + i);
        displayTune();
    }
    else if (focusedFieldName == "velo-pitch")
    {
        selectedNoteParameters->setVelocityToPitch(
            selectedNoteParameters->getVelocityToPitch() + i);
        displayVeloPitch();
    }
    else if (focusedFieldName == "note")
    {
        mpc.clientEventController->setSelectedNote(
            mpc.clientEventController->getSelectedNote() + i);
        // We could call all display methods here, but we instead rely on the
        // "note" message
    }
}

void VeloPitchScreen::displayTune()
{
    auto program = getProgramOrThrow();
    auto selectedNoteParameters = program->getNoteParameters(
        mpc.clientEventController->getSelectedNote());
    auto value = selectedNoteParameters->getTune();
    std::string prefix = value < 0 ? "-" : " ";
    findField("tune")->setText(
        prefix + StrUtil::padLeft(std::to_string(abs(value)), " ", 3));
}

void VeloPitchScreen::displayVeloPitch()
{
    auto program = getProgramOrThrow();
    auto selectedNoteParameters = program->getNoteParameters(
        mpc.clientEventController->getSelectedNote());
    auto value = selectedNoteParameters->getVelocityToPitch();
    std::string prefix = value < 0 ? "-" : " ";
    findField("velo-pitch")
        ->setText(prefix +
                  StrUtil::padLeft(std::to_string(abs(value)), " ", 3));
}

void VeloPitchScreen::displayVelo()
{
    findField("velo")->setText("127");
}

void VeloPitchScreen::update(Observable *observable, Message message)
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
