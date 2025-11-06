#include "VeloPitchScreen.hpp"

#include "Mpc.hpp"
#include "StrUtil.hpp"
#include "controller/ClientEventController.hpp"
#include "sampler/NoteParameters.hpp"
#include "sampler/Program.hpp"
#include "sampler/Sampler.hpp"

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
    const auto program = getProgramOrThrow();
    const auto selectedNoteParameters = program->getNoteParameters(
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
    const auto program = getProgramOrThrow();
    const auto selectedNoteParameters = program->getNoteParameters(
        mpc.clientEventController->getSelectedNote());
    const auto value = selectedNoteParameters->getTune();
    const std::string prefix = value < 0 ? "-" : " ";
    findField("tune")->setText(
        prefix + StrUtil::padLeft(std::to_string(abs(value)), " ", 3));
}

void VeloPitchScreen::displayVeloPitch()
{
    const auto program = getProgramOrThrow();
    const auto selectedNoteParameters = program->getNoteParameters(
        mpc.clientEventController->getSelectedNote());
    const auto value = selectedNoteParameters->getVelocityToPitch();
    const std::string prefix = value < 0 ? "-" : " ";
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
