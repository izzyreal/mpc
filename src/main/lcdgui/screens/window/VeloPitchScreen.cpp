#include "VeloPitchScreen.hpp"

#include "Mpc.hpp"
#include "StrUtil.hpp"
#include "controller/ClientEventController.hpp"
#include "controller/ClientHardwareEventController.hpp"
#include "sampler/NoteParameters.hpp"
#include "sampler/Program.hpp"
#include "sampler/Sampler.hpp"

using namespace mpc::lcdgui::screens::window;

VeloPitchScreen::VeloPitchScreen(Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "velo-pitch", layerIndex) {
    addReactiveBinding({
        [this] {
            const auto controller = this->mpc.clientEventController->clientHardwareEventController;
            return controller->getMostRecentPhysicalPadPressTime();
        }, [this](auto) mutable {
            const auto controller = this->mpc.clientEventController->clientHardwareEventController;
            setVelo(controller->getMostRecentPhysicalPadPressVelocity());
        }
    });
}

void VeloPitchScreen::open()
{
    setVelo(MaxVelocity);
    if (isReactiveBindingsEmpty())
    {
        auto getSelectedNote = [this]
        {
            return this->mpc.clientEventController->getSelectedNote();
        };

        auto getSelectedNoteParameters = [this, getSelectedNote]
        {
            return getProgramOrThrow()->getNoteParameters(getSelectedNote());
        };

        addReactiveBinding({[getSelectedNote]
                            {
                                return getSelectedNote();
                            },
                            [this](auto)
                            {
                                displayNote();
                                displayTune();
                                displayVelo();
                            }});

        addReactiveBinding({[getSelectedNoteParameters]
                            {
                                return getSelectedNoteParameters()->getTune();
                            },
                            [this](auto)
                            {
                                displayTune();
                            }});

        addReactiveBinding(
            {[getSelectedNoteParameters]
             {
                 return getSelectedNoteParameters()->getVelocityToPitch();
             },
             [this](auto)
             {
                 displayVeloPitch();
             }});
    }

    displayNote();
    displayTune();
    displayVeloPitch();

    displayVelo();
}

void VeloPitchScreen::turnWheel(const int i)
{
    const auto program = getProgramOrThrow();
    const auto selectedNoteParameters = program->getNoteParameters(
        mpc.clientEventController->getSelectedNote());

    if (const auto focusedFieldName = getFocusedFieldNameOrThrow(); focusedFieldName == "tune")
    {
        selectedNoteParameters->setTune(selectedNoteParameters->getTune() + i);
    }
    else if (focusedFieldName == "velo-pitch")
    {
        selectedNoteParameters->setVelocityToPitch(
            selectedNoteParameters->getVelocityToPitch() + i);
    }
    else if (focusedFieldName == "note")
    {
        mpc.clientEventController->setSelectedNote(
            mpc.clientEventController->getSelectedNote() + i);
    }
    else if (focusedFieldName == "velo")
    {
        setVelo(velo + i);
    }
}

void VeloPitchScreen::displayTune() const
{
    const auto program = getProgramOrThrow();
    const auto selectedNoteParameters = program->getNoteParameters(
        mpc.clientEventController->getSelectedNote());
    const auto value = selectedNoteParameters->getTune();
    const std::string prefix = value < 0 ? "-" : " ";
    findField("tune")->setText(
        prefix + StrUtil::padLeft(std::to_string(abs(value)), " ", 3));
}

void VeloPitchScreen::displayVeloPitch() const
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

void VeloPitchScreen::displayVelo() const
{
    findField("velo")->setTextPadded(velo);
}

void VeloPitchScreen::displayNote() const
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

void VeloPitchScreen::setVelo(const Velocity newVelo)
{
    velo = std::clamp(newVelo, Velocity1, MaxVelocity);
    displayVelo();
}