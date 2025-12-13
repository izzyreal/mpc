#include "LoopSongScreen.hpp"

#include "Mpc.hpp"

#include "sequencer/Sequencer.hpp"
#include "sequencer/Song.hpp"

using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens;

LoopSongScreen::LoopSongScreen(Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "loop-song", layerIndex)
{
    addReactiveBinding({[&]
                        {
                            return sequencer.lock()
                                ->getSelectedSong()
                                ->getFirstLoopStepIndex();
                        },
                        [&](auto)
                        {
                            displayFirstStep();
                            displayNumberOfSteps();
                        }});
    addReactiveBinding({[&]
                        {
                            return sequencer.lock()
                                ->getSelectedSong()
                                ->getLastLoopStepIndex();
                        },
                        [&](auto)
                        {
                            displayLastStep();
                            displayNumberOfSteps();
                        }});
}

void LoopSongScreen::open()
{
    displayFirstStep();
    displayLastStep();
    displayNumberOfSteps();
}

void LoopSongScreen::turnWheel(const int i)
{
    const auto song = sequencer.lock()->getSelectedSong();

    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName == "first-step")
    {
        song->setFirstLoopStepIndex(song->getFirstLoopStepIndex() + i);
    }
    else if (focusedFieldName == "last-step")
    {
        song->setLastLoopStepIndex(song->getLastLoopStepIndex() + i);
    }
    else if (focusedFieldName == "number-of-steps")
    {
        const auto candidate = song->getLastLoopStepIndex() + i;

        if (candidate < song->getFirstLoopStepIndex())
        {
            return;
        }

        song->setLastLoopStepIndex(candidate);
    }
}

void LoopSongScreen::displayFirstStep() const
{
    const auto song = sequencer.lock()->getSelectedSong();
    findField("first-step")
        ->setTextPadded(std::to_string(song->getFirstLoopStepIndex() + 1));
}

void LoopSongScreen::displayLastStep() const
{
    const auto song = sequencer.lock()->getSelectedSong();
    findField("last-step")
        ->setTextPadded(std::to_string(song->getLastLoopStepIndex() + 1));
}

void LoopSongScreen::displayNumberOfSteps() const
{
    const auto song = sequencer.lock()->getSelectedSong();
    findField("number-of-steps")
        ->setTextPadded(std::to_string(song->getLastLoopStepIndex() -
                                       song->getFirstLoopStepIndex() + 1));
}
