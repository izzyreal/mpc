#include "KeepOrRetryScreen.hpp"

#include "Mpc.hpp"
#include "controller/ClientEventController.hpp"
#include "lcdgui/screens/window/NameScreen.hpp"
#include "sampler/Sampler.hpp"

using namespace mpc::lcdgui::screens::window;

KeepOrRetryScreen::KeepOrRetryScreen(Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "keep-or-retry", layerIndex)
{
}

void KeepOrRetryScreen::open()
{
    if (ls->isPreviousScreenNot({ScreenId::NameScreen}))
    {
        assignToNote = NoDrumNoteAssigned;
    }
    displayNameForNewSound();
    displayAssignToNote();
    mpc.clientEventController->addObserver(this); // Subscribe to "note" message
}

void KeepOrRetryScreen::close()
{
    mpc.clientEventController->deleteObserver(this);
}

void KeepOrRetryScreen::function(const int i)
{
    switch (i)
    {
        case 1:
            sampler->deleteSound(sampler->getPreviewSound());
            openScreenById(ScreenId::SampleScreen);
            break;
        case 3:
            sampler->playPreviewSample(
                0, sampler->getPreviewSound()->getLastFrameIndex(), 0);
            break;
        case 4:
        {
            const auto index = sampler->getSoundCount() - 1;

            if (assignToNote != NoDrumNoteAssigned)
            {
                getProgramOrThrow()
                    ->getNoteParameters(assignToNote)
                    ->setSoundIndex(index);
            }

            sampler->setSoundIndex(index);
            openScreenById(ScreenId::SampleScreen);
            break;
        }
        default:;
    }
}

void KeepOrRetryScreen::openNameScreen()
{
    if (const auto focusedFieldName = getFocusedFieldNameOrThrow();
        focusedFieldName == "name-for-new-sound")
    {
        const auto enterAction = [this](const std::string &nameScreenName)
        {
            if (mpc.getSampler()->isSoundNameOccupied(nameScreenName))
            {
                return;
            }

            sampler->getPreviewSound()->setName(nameScreenName);
            openScreenById(ScreenId::KeepOrRetryScreen);
        };

        const auto nameScreen = mpc.screens->get<ScreenId::NameScreen>();
        nameScreen->initialize(sampler->getPreviewSound()->getName(), 16,
                               enterAction, "keep-or-retry");
        openScreenById(ScreenId::NameScreen);
    }
}

void KeepOrRetryScreen::right()
{
    if (const auto focusedFieldName = getFocusedFieldNameOrThrow();
        focusedFieldName == "name-for-new-sound")
    {
        openNameScreen();
    }
}

void KeepOrRetryScreen::turnWheel(const int increment)
{
    if (const auto focusedFieldName = getFocusedFieldNameOrThrow();
        focusedFieldName == "assign-to-note")
    {
        auto newAssignToNote = assignToNote + increment;

        if (newAssignToNote < NoDrumNoteAssigned)
        {
            newAssignToNote = NoDrumNoteAssigned;
        }
        else if (newAssignToNote > MaxDrumNoteNumber)
        {
            newAssignToNote = MaxDrumNoteNumber;
        }

        if (newAssignToNote == NoDrumNoteAssigned)
        {
            mpc.clientEventController->setSelectedNote(MinDrumNoteNumber);
            assignToNote = newAssignToNote;
            displayAssignToNote();
        }
        else
        {
            mpc.clientEventController->setSelectedNote(newAssignToNote);
        }
    }
}

void KeepOrRetryScreen::displayNameForNewSound() const
{
    if (!sampler->getSound())
    {
        return;
    }

    findField("name-for-new-sound")
        ->setText(sampler->getPreviewSound()->getName());
}

void KeepOrRetryScreen::displayAssignToNote() const
{
    const auto noteStr = assignToNote == NoDrumNoteAssigned
                             ? "--"
                             : std::to_string(assignToNote);
    const auto padStr = sampler->getPadName(
        getProgramOrThrow()->getPadIndexFromNote(assignToNote));
    findField("assign-to-note")->setText(noteStr + "/" + padStr);
}

void KeepOrRetryScreen::update(Observable *o, const Message message)
{
    if (const auto msg = std::get<std::string>(message); msg == "note")
    {
        assignToNote = mpc.clientEventController->getSelectedNote();
        displayAssignToNote();
    }
}
