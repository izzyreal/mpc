#include "VelocityModulationScreen.hpp"

#include "Mpc.hpp"
#include "StrUtil.hpp"
#include "controller/ClientEventController.hpp"
#include "controller/ClientHardwareEventController.hpp"
#include "sampler/Sampler.hpp"

using namespace mpc::lcdgui::screens::window;

VelocityModulationScreen::VelocityModulationScreen(Mpc &mpc,
                                                   const int layerIndex)
    : ScreenComponent(mpc, "velocity-modulation", layerIndex)
{
}

void VelocityModulationScreen::open()
{
    setVelo(MaxVelocity);

    auto displayNoteProperties = [this]
    {
        displayNote();
        displayVeloAttack();
        displayVeloStart();
        displayVeloLevel();
    };

    displayNoteProperties();
    displayVelo();

    auto getSelectedNote = [this]
    {
        return this->mpc.clientEventController->getSelectedNote();
    };

    auto getSelectedNoteParameters = [this, getSelectedNote]
    {
        return getProgramOrThrow()->getNoteParameters(getSelectedNote());
    };

    if (isReactiveBindingsEmpty())
    {
        addReactiveBinding(
            {[this]
             {
                 const auto controller = this->mpc.clientEventController
                                             ->clientHardwareEventController;
                 return controller->getMostRecentPhysicalPadPressTime();
             },
             [this](auto) mutable
             {
                 const auto controller = this->mpc.clientEventController
                                             ->clientHardwareEventController;
                 setVelo(controller->getMostRecentPhysicalPadPressVelocity());
             }});

        addReactiveBinding({[getSelectedNote]
                            {
                                return getSelectedNote();
                            },
                            [displayNoteProperties](auto)
                            {
                                displayNoteProperties();
                            }});

        addReactiveBinding(
            {[getSelectedNoteParameters]
             {
                 return getSelectedNoteParameters()->getVelocityToAttack();
             },
             [this](auto)
             {
                 displayVeloAttack();
             }});
        addReactiveBinding(
            {[getSelectedNoteParameters]
             {
                 return getSelectedNoteParameters()->getVelocityToStart();
             },
             [this](auto)
             {
                 displayVeloStart();
             }});
        addReactiveBinding(
            {[getSelectedNoteParameters]
             {
                 return getSelectedNoteParameters()->getVeloToLevel();
             },
             [this](auto)
             {
                 displayVeloLevel();
             }});
    }
}

void VelocityModulationScreen::turnWheel(const int i)
{
    const auto program = getProgramOrThrow();
    const auto selectedNoteParameters = program->getNoteParameters(
        mpc.clientEventController->getSelectedNote());

    if (const auto focusedFieldName = getFocusedFieldNameOrThrow();
        focusedFieldName == "veloattack")
    {
        selectedNoteParameters->setVelocityToAttack(
            selectedNoteParameters->getVelocityToAttack() + i);
    }
    else if (focusedFieldName == "velostart")
    {
        selectedNoteParameters->setVelocityToStart(
            selectedNoteParameters->getVelocityToStart() + i);
    }
    else if (focusedFieldName == "velolevel")
    {
        selectedNoteParameters->setVeloToLevel(
            selectedNoteParameters->getVeloToLevel() + i);
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

void VelocityModulationScreen::displayNote() const
{
    const auto program = getProgramOrThrow();
    const auto selectedNoteParameters = program->getNoteParameters(
        mpc.clientEventController->getSelectedNote());
    const auto soundIndex = selectedNoteParameters->getSoundIndex();
    const auto padIndex =
        program->getPadIndexFromNote(selectedNoteParameters->getNumber());
    const auto padName = sampler.lock()->getPadName(padIndex);
    const auto sampleName =
        soundIndex != -1 ? sampler.lock()->getSoundName(soundIndex) : "OFF";
    const std::string stereo =
        soundIndex != -1 && !sampler.lock()->getSound(soundIndex)->isMono()
            ? "(ST)"
            : "";
    findField("note")->setText(
        std::to_string(selectedNoteParameters->getNumber()) + "/" + padName +
        "-" + StrUtil::padRight(sampleName, " ", 16) + stereo);
}

void VelocityModulationScreen::setVelo(const Velocity newVelo)
{
    velo = std::clamp(newVelo, Velocity1, MaxVelocity);
    displayVelo();
}

void VelocityModulationScreen::displayVelo() const
{
    findField("velo")->setTextPadded(velo);
}

void VelocityModulationScreen::displayVeloAttack() const
{
    const auto program = getProgramOrThrow();
    const auto selectedNoteParameters = program->getNoteParameters(
        mpc.clientEventController->getSelectedNote());
    findField("veloattack")
        ->setTextPadded(selectedNoteParameters->getVelocityToAttack(), " ");
}

void VelocityModulationScreen::displayVeloStart() const
{
    const auto program = getProgramOrThrow();
    const auto selectedNoteParameters = program->getNoteParameters(
        mpc.clientEventController->getSelectedNote());
    findField("velostart")
        ->setTextPadded(selectedNoteParameters->getVelocityToStart(), " ");
}

void VelocityModulationScreen::displayVeloLevel() const
{
    const auto program = getProgramOrThrow();
    const auto selectedNoteParameters = program->getNoteParameters(
        mpc.clientEventController->getSelectedNote());
    findField("velolevel")
        ->setTextPadded(selectedNoteParameters->getVeloToLevel(), " ");
}
