#include "VeloEnvFilterScreen.hpp"

#include "Mpc.hpp"
#include "StrUtil.hpp"
#include "controller/ClientEventController.hpp"
#include "controller/ClientHardwareEventController.hpp"
#include "sampler/Sampler.hpp"

using namespace mpc::lcdgui::screens::window;

VeloEnvFilterScreen::VeloEnvFilterScreen(Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "velo-env-filter", layerIndex)
{
    addChildT<EnvGraph>(mpc);
    addReactiveBinding(
        {[this]
         {
             const auto controller =
                 this->mpc.clientEventController->clientHardwareEventController;
             return controller->getMostRecentPhysicalPadPressTime();
         },
         [this](auto) mutable
         {
             const auto controller =
                 this->mpc.clientEventController->clientHardwareEventController;
             setVelo(controller->getMostRecentPhysicalPadPressVelocity());
         }});
}

void VeloEnvFilterScreen::open()
{
    setVelo(MaxVelocity);
    auto displayNoteProperties = [this]
    {
        displayNote();
        displayAttack();
        displayDecay();
        displayAmount();
        displayVeloFreq();
    };

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
                 return getSelectedNoteParameters()->getFilterAttack();
             },
             [this](auto)
             {
                 displayAttack();
             }});
        addReactiveBinding(
            {[getSelectedNoteParameters]
             {
                 return getSelectedNoteParameters()->getFilterDecay();
             },
             [this](auto)
             {
                 displayDecay();
             }});
        addReactiveBinding(
            {[getSelectedNoteParameters]
             {
                 return getSelectedNoteParameters()->getFilterEnvelopeAmount();
             },
             [this](auto)
             {
                 displayAmount();
             }});
        addReactiveBinding({[getSelectedNoteParameters]
                            {
                                return getSelectedNoteParameters()
                                    ->getVelocityToFilterFrequency();
                            },
                            [this](auto)
                            {
                                displayVeloFreq();
                            }});
    }

    displayNoteProperties();
    displayVelo();
}

void VeloEnvFilterScreen::turnWheel(const int i)
{
    const auto program = getProgramOrThrow();
    const auto selectedNoteParameters = program->getNoteParameters(
        mpc.clientEventController->getSelectedNote());

    if (const auto focusedFieldName = getFocusedFieldNameOrThrow();
        focusedFieldName == "attack")
    {
        selectedNoteParameters->setFilterAttack(
            selectedNoteParameters->getFilterAttack() + i);
    }
    else if (focusedFieldName == "decay")
    {
        selectedNoteParameters->setFilterDecay(
            selectedNoteParameters->getFilterDecay() + i);
    }
    else if (focusedFieldName == "amount")
    {
        selectedNoteParameters->setFilterEnvelopeAmount(
            selectedNoteParameters->getFilterEnvelopeAmount() + i);
    }
    else if (focusedFieldName == "velofreq")
    {
        selectedNoteParameters->setVelocityToFilterFrequency(
            selectedNoteParameters->getVelocityToFilterFrequency() + i);
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

void VeloEnvFilterScreen::displayNote() const
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

void VeloEnvFilterScreen::displayVelo() const
{
    findField("velo")->setTextPadded(velo);
}

void VeloEnvFilterScreen::displayAttack()
{
    const auto program = getProgramOrThrow();
    const auto selectedNoteParameters = program->getNoteParameters(
        mpc.clientEventController->getSelectedNote());
    const auto attack = selectedNoteParameters->getFilterAttack();
    const auto decay = selectedNoteParameters->getFilterDecay();
    findField("attack")->setTextPadded(attack, " ");
    findEnvGraph()->setCoordinates(attack, decay, true);
}

void VeloEnvFilterScreen::displayDecay()
{
    const auto program = getProgramOrThrow();
    const auto selectedNoteParameters = program->getNoteParameters(
        mpc.clientEventController->getSelectedNote());
    const auto attack = selectedNoteParameters->getFilterAttack();
    const auto decay = selectedNoteParameters->getFilterDecay();
    findField("decay")->setTextPadded(decay, " ");
    findEnvGraph()->setCoordinates(attack, decay, true);
}

void VeloEnvFilterScreen::displayAmount() const
{
    const auto program = getProgramOrThrow();
    const auto selectedNoteParameters = program->getNoteParameters(
        mpc.clientEventController->getSelectedNote());
    findField("amount")->setTextPadded(
        selectedNoteParameters->getFilterEnvelopeAmount(), " ");
}

void VeloEnvFilterScreen::displayVeloFreq() const
{
    const auto program = getProgramOrThrow();
    const auto selectedNoteParameters = program->getNoteParameters(
        mpc.clientEventController->getSelectedNote());
    findField("velofreq")
        ->setTextPadded(selectedNoteParameters->getVelocityToFilterFrequency(),
                        " ");
}

void VeloEnvFilterScreen::setVelo(const Velocity newVelo)
{
    velo = std::clamp(newVelo, Velocity1, MaxVelocity);
    displayVelo();
}
