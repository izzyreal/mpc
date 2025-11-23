#include "LoopSongScreen.hpp"

#include "Mpc.hpp"
#include "lcdgui/screens/SongScreen.hpp"

#include "sequencer/Sequencer.hpp"
#include "sequencer/Song.hpp"

using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens;

LoopSongScreen::LoopSongScreen(Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "loop-song", layerIndex)
{
}

void LoopSongScreen::open()
{
    displayFirstStep();
    displayLastStep();
    displayNumberOfSteps();
}

void LoopSongScreen::turnWheel(const int i)
{
    const auto songScreen = mpc.screens->get<ScreenId::SongScreen>();
    const auto song =
        sequencer.lock()->getSong(songScreen->getSelectedSongIndex());

    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName == "first-step")
    {
        song->setFirstStep(song->getFirstStep() + i);
        displayFirstStep();
        displayLastStep();
        displayNumberOfSteps();
    }
    else if (focusedFieldName == "last-step")
    {
        song->setLastStep(song->getLastStep() + i);
        displayLastStep();
        displayFirstStep();
        displayNumberOfSteps();
    }
    else if (focusedFieldName == "number-of-steps")
    {
        const auto candidate = song->getLastStep() + i;

        if (candidate < song->getFirstStep())
        {
            return;
        }

        song->setLastStep(candidate);

        displayLastStep();
        displayNumberOfSteps();
    }
}

void LoopSongScreen::displayFirstStep() const
{
    const auto songScreen = mpc.screens->get<ScreenId::SongScreen>();
    const auto song =
        sequencer.lock()->getSong(songScreen->getSelectedSongIndex());
    findField("first-step")
        ->setTextPadded(std::to_string(song->getFirstStep() + 1));
}

void LoopSongScreen::displayLastStep() const
{
    const auto songScreen = mpc.screens->get<ScreenId::SongScreen>();
    const auto song =
        sequencer.lock()->getSong(songScreen->getSelectedSongIndex());

    findField("last-step")
        ->setTextPadded(std::to_string(song->getLastStep() + 1));
}

void LoopSongScreen::displayNumberOfSteps() const
{
    const auto songScreen = mpc.screens->get<ScreenId::SongScreen>();
    const auto song =
        sequencer.lock()->getSong(songScreen->getSelectedSongIndex());
    findField("number-of-steps")
        ->setTextPadded(
            std::to_string(song->getLastStep() - song->getFirstStep() + 1));
}
